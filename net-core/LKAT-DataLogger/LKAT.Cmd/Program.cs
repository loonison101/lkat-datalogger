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

    [Verb("verify-gpx", HelpText = "Verify status of gpx files are persisted")]
    public class VerifyOptions
    {
        [Option('d', "sourceDirectory", Required = true, HelpText = "Validate gpx files exist in database and match")]
        public string Directory { get; set; }
    }

    [Verb("verify-csv", HelpText = "Verify status of csv file(s). i.e. do they have a header, etc...")]
    public class CsvModifyOptions
    {
        [Option('y', "shouldWrite", Required = false, HelpText = "Defaults to false. Won't actually change file(s) unless this is true, otherwise it will show what it could change")]
        public bool ShouldWrite { get; set; }
    }

    [Verb("db-count", HelpText = "Gives you stats about the CSV database")]
    public class DbCountOptions
    {
        
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
            var loaders = new List<IFileLoader>()
            {
                new LocalFileLoader(log),
                new WebFileLoader(log)
            };

            var service = new CsvLoaderService(log, loaders);
            var exporterService = new CsvExporterService(log);

            var fileMetaService = new FileMetaService(Environment.GetEnvironmentVariable("LKAT-DATALOGGER-APIKEY"),
                Environment.GetEnvironmentVariable("LKAT-DATALOGGER-DBID"));
            var fileMetaValidator = new FileMetaValidatorService(fileMetaService, log);

            var csvModifierService = new CsvModifierService(log);

            var csvRepo = new CsvRepository();

            return CommandLine.Parser.Default.ParseArguments<GpxOptions, LoadOptions, VerifyOptions, CsvModifyOptions, DbCountOptions>(args)
                .MapResult(
                    (GpxOptions opts) => RunGpx(opts, exporterService),
                    (LoadOptions opts) => RunLoad(opts, service, log),
                    (VerifyOptions opts) => RunGpxVerify(opts, fileMetaValidator, log),
                    (CsvModifyOptions opts) => RunCsvVerify(opts, log, csvModifierService),
                    (DbCountOptions opts) => RunDbCount(opts, log, csvRepo),
                    errs => 1);
                    
        }

        private static int RunDbCount(DbCountOptions opts, Logger log, CsvRepository csvRepo)
        {
            log.Information("Using this configuration: {0}", JsonConvert.SerializeObject(opts));
            log.Information("Querying...");
            var count = csvRepo.RecordCount();
            log.Information("CSV Record Count: {0}", count);
            return 0;
        }

        private static int RunGpxVerify(VerifyOptions opts, FileMetaValidatorService service, Logger log)
        {
            log.Information("Using this configuration: {0}", JsonConvert.SerializeObject(opts));
            service.SyncAndValidate(opts.Directory);
            return 0;
        }

        private static int RunCsvVerify(CsvModifyOptions opts, Logger log, CsvModifierService service)
        {
            log.Information("Using this configuration: {0}", JsonConvert.SerializeObject(opts));
            service.AddHeaderToFiles(opts);
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