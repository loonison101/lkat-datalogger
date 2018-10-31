using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using LKAT.Cmd;
using Microsoft.Extensions.Logging;
using Serilog;
using Serilog.Core;
using Xunit;

namespace LKAT.Test
{
    public class UnitTest1
    {
        public UnitTest1(Logger log)
        {
            _log = log;
        }

        private Logger _log;

        [Fact]
        public void DownloadCsvAndLoadDb()
        {
            var log = Mocks.Logger();
            var loaders = Mocks.Loaders();
            
            var service = new CsvLoaderService(log, loaders);

            var options = new LoadOptions()
            {
                // Linux path
//                sourceFile = "/mnt/26188D9B188D6A9F/Users/looni/Documents/GitHub/lkat-datalogger/generated/ep62srJig-v2.csv"
                sourceFile = @"C:\Users\looni\Documents\GitHub\lkat-datalogger\generated\ep62srJig-v2.csv"
            };
            
            // Download the file
            service.LoadFileIntoDb(options);
            //
        }

        [Fact]
        public void CreateGpx()
        {
            var log = Mocks.Logger();
            var service = new CsvExporterService(log);

            //string filePath = service.Export();
            service.Export();

            //Assert.True(File.Exists(filePath));
        }

        /**
         * We need a way that once gpx files are created, that there are records in a DB to keep track of them
         * If a hash changed, we need to know about it
         **/
        [Fact]
        public void PersistGpxWorkflow()
        {
            var options = new VerifyOptions()
            {
                Directory = @"C:\Users\looni\AppData\Local\Temp\gpx"
            };

            // Load the files from our directory we'll be evaluating
            var service = new FileMetaService(Environment.GetEnvironmentVariable("LKAT-DATALOGGER-APIKEY"),
                Environment.GetEnvironmentVariable("LKAT-DATALOGGER-DBID"));
            var validator = new FileMetaValidatorService(service, _log);

            validator.SyncAndValidate(options.Directory);
        }
    }
}