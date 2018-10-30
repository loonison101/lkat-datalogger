using System;
using System.Collections.Generic;
using System.IO;
using LKAT.Cmd;
using Microsoft.Extensions.Logging;
using Serilog;
using Xunit;

namespace LKAT.Test
{
    public class UnitTest1
    {
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
    }
}