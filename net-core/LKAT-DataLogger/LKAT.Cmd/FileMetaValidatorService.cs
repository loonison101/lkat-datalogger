using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Serilog.Core;

namespace LKAT.Cmd
{
    public class FileMetaValidatorService
    {
        private readonly FileMetaService _service;
        private readonly Logger _logger;

        public FileMetaValidatorService(FileMetaService service, Logger logger)
        {
            this._service = service;
            _logger = logger;
        }

        public void SyncAndValidate(string directory)
        {
            _logger.Information("Beginning sync and validation");

            List<FileMeta> files = _service.GetLocal(directory);
            _logger.Information("Found {0} files from {1}", files.Count, directory);
            List<FileMeta> dbFiles = _service.GetFromDb();
            _logger.Information("Found {0} files from DB", dbFiles.Count);

            // Any local not in db?
            var filesToCreate = files.Where(x => dbFiles.All(db => db.NameWithExtension != x.NameWithExtension)).ToList();
            if (filesToCreate.Any())
            {
                _service.InsertFilesToDb(filesToCreate);
                _logger.Information("Created {0} files", filesToCreate.Count);
                dbFiles = _service.GetFromDb();
            }
            else
            {
                _logger.Information("No files need created");
            }
            
            // Any in db not local?
            var filesOutOfSync = dbFiles.Where(x => files.All(f => f.NameWithExtension != x.NameWithExtension)).ToList();
            if (filesOutOfSync.Any())
                throw new Exception("There are gpx files in db not local, bad news bears");

            // Any hash differences?
            var filesWithHashDifferences = files.Where(f => dbFiles.Any(d => f.NameWithExtension == d.NameWithExtension && f.Hash != d.Hash)).ToList();
            if (filesWithHashDifferences.Any())
                throw new Exception("There are gpx files with different hashes");

            _logger.Information("Done validating");
        }
    }
}
