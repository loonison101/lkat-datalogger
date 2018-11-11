using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using Google.Apis.Upload;
using Google.Cloud.Storage.V1;
using Serilog.Core;

namespace LKAT.Cmd
{
    public class BackupService
    {
        private readonly Logger _logger;

        public BackupService(Logger logger)
        {
            _logger = logger;
        }
        public void BackupDb(BackupDbOptions opts)
        {
            //string projectId = Environment.GetEnvironmentVariable("LKAT-DATALOGGER-GCS-PROJECT-ID");
            StorageClient client = StorageClient.Create();
            string extension = Path.GetExtension(opts.DbPath);

            using (var f = File.OpenRead(opts.DbPath))
            {
                var objectName = "dbs/lkat-backup-" + DateTime.Now.ToString("MM-dd-yyyy") + "-" + DateTime.Now.Ticks + extension;
                IProgress<IUploadProgress> progress = new Progress<IUploadProgress>(
                    delegate (IUploadProgress uploadProgress)
                    {
                        //Console.WriteLine(uploadProgress.BytesSent);
                        _logger.Debug("Uploading file: {0}", uploadProgress.BytesSent);
                    });

                _logger.Information("Beginning backup named: {0}", objectName);
                var result = client.UploadObject("lkat-datalogger", objectName, "application/octet-stream", f, null, progress);
                _logger.Information("File backed up");
            }
        }
    }
}
