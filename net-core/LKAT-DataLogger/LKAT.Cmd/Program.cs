using System;
using System.Collections.Generic;
using CommandLine;
using Newtonsoft.Json;
using Serilog;
using Serilog.Core;

namespace LKAT.Cmd
{
    [Verb("gpx", HelpText = "Create a GPX file.")]
    public class GpxOptions
    {
        
    }

    [Verb("load", HelpText = "Take a CSV file and load into DB")]
    public class LoadOptions
    {
        [Option('s', "sourceFile", Required = true, HelpText = "Source url/path to csv file to consume")]
        public string sourceFile { get; set; }
    }

    [Verb("verify", HelpText = "Verify status of gpx files are persisted")]
    public class VerifyOptions
    {
        [Option('d', "sourceDirectory", Required = true, HelpText = "Validate gpx files exist in database and match")]
        public string Directory { get; set; }
    }
    
    class Program
    {
        static int Main(string[] args)
        {
            var log = new LoggerConfiguration()
                .MinimumLevel.Debug()
                .WriteTo.Console()
                .WriteTo.File("logs\\log.txt", rollingInterval: RollingInterval.Month )
                .CreateLogger();


            if (string.IsNullOrWhiteSpace(Environment.GetEnvironmentVariable("LKAT_DATALOGGER_APIKEY"))) {
                log.Error("LKAT_DATALOGGER_APIKEY was not found in your environment variables");
                return 1;
            }

            if (string.IsNullOrWhiteSpace(Environment.GetEnvironmentVariable("LKAT_DATALOGGER_DBID"))) {
                log.Error("LKAT_DATALOGGER_DBID was not found in your environment variables");
                return 1;
            }

            var loaders = new List<IFileLoader>()
            {
                new LocalFileLoader(log),
                new WebFileLoader(log)
            };

            var service = new CsvLoaderService(log, loaders);
            var exporterService = new CsvExporterService(log);

            var fileMetaService = new FileMetaService(Environment.GetEnvironmentVariable("LKAT_DATALOGGER_APIKEY"),
                Environment.GetEnvironmentVariable("LKAT_DATALOGGER_DBID"));
            var fileMetaValidator = new FileMetaValidatorService(fileMetaService, log);

            return CommandLine.Parser.Default.ParseArguments<GpxOptions, LoadOptions, VerifyOptions>(args)
                .MapResult(
                    (GpxOptions opts) => RunGpx(opts, exporterService),
                    (LoadOptions opts) => RunLoad(opts, service, log),
                    (VerifyOptions opts) => RunVerify(opts, fileMetaValidator, log),
                    errs => 1);
                    
        }

        private static int RunVerify(VerifyOptions opts, FileMetaValidatorService service, Logger log)
        {
            log.Information("Using this configuration: {0}", JsonConvert.SerializeObject(opts));
            service.SyncAndValidate(opts.Directory);
            return 0;
        }

        private static int RunLoad(LoadOptions opts, CsvLoaderService service, Serilog.Core.Logger log)
        {
            log.Information("Using this configuration: {0}", JsonConvert.SerializeObject(opts));
            service.LoadFileIntoDb(opts);
            return 0;
        }

        private static int RunGpx(GpxOptions opts, CsvExporterService exporterService)
        {
            exporterService.Export();
            return 0;
        }
    }
}