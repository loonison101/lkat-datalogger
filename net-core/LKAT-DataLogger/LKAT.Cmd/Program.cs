using System;
using System.Collections.Generic;
using System.IO;
using CommandLine;
using Newtonsoft.Json;
using Serilog;
using Serilog.Core;
using Serilog.Exceptions;

namespace LKAT.Cmd
{
    public static class CONSTANTS
    {
        public static string DB_PATH = @"./lkat.db";
        public static string LOG_PATH = @"logs\log.txt";
    }

    public abstract class DatabaseOptions
    {
        [Option("db", HelpText = "Path to database")]
        public string DbPath { get; set; }

        protected DatabaseOptions()
        {
            //DbPath = @"C:\Users\looni\Documents\GitHub\lkat-datalogger\net-core\LKAT-DataLogger\LKAT.Cmd\lkat.db";
            DbPath = CONSTANTS.DB_PATH;
        }
    }

    [Verb("gpx", HelpText = "Create a GPX file.")]
    public class GpxOptions : DatabaseOptions
    {
        
    }

    [Verb("load", HelpText = "Take a CSV file and load into DB")]
    public class LoadOptions : DatabaseOptions
    {
        [Option('s', "sourceFile", Required = true, HelpText = "Source url/path to csv file to consume")]
        public string sourceFile { get; set; }
    }

    [Verb("verify-gpx", HelpText = "Verify status of gpx files are persisted")]
    public class VerifyOptions : DatabaseOptions
    {
        [Option('d', "sourceDirectory", Required = true, HelpText = "Validate gpx files exist in database and match")]
        public string Directory { get; set; }
    }

    [Verb("verify-csv", HelpText = "Verify status of csv file(s). i.e. do they have a header, etc...")]
    public class CsvModifyOptions : DatabaseOptions
    {
        [Option('y', "shouldWrite", Required = false, HelpText = "Defaults to false. Won't actually change file(s) unless this is true, otherwise it will show what it could change")]
        public bool ShouldWrite { get; set; }
    }

    [Verb("db-count", HelpText = "Gives you stats about the CSV database")]
    public class DbCountOptions : DatabaseOptions
    {
        
    }

    [Verb("backup-db", HelpText = "Backups CSV database to GCS")]
    public class BackupDbOptions : DatabaseOptions
    {
        
    }

    public class ApplicationPreStartupException : Exception
    {
        public ApplicationPreStartupException(string message) : base(message)
        {
        }
    }

    class Program
    {
        static int Main(string[] args)
        {
            var log = new LoggerConfiguration()
                .Enrich.WithExceptionDetails()
                .MinimumLevel.Debug()
                .WriteTo.Console()
                .WriteTo.File(CONSTANTS.LOG_PATH, rollingInterval: RollingInterval.Month )
                .CreateLogger();

            // Google cloud key, check if it is set/exists
            if (string.IsNullOrWhiteSpace(Environment.GetEnvironmentVariable("GOOGLE_APPLICATION_CREDENTIALS")))
            {
                var ex = new ApplicationPreStartupException("GOOGLE_APPLICATION_CREDENTIALS environment variable needs to exist");
                log.Error(ex, "Failed startup step 1");
                return 1;
            }

            // Does the db exist?
            if (!File.Exists(CONSTANTS.DB_PATH))
            {
                var ex = new ApplicationPreStartupException(CONSTANTS.DB_PATH + " must exist for this app to run properly");
                log.Error(ex, "Failed startup step 2");
                return 1;
            }

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

            var backupService = new BackupService(log);

            return CommandLine.Parser.Default.ParseArguments<GpxOptions, LoadOptions, VerifyOptions, CsvModifyOptions, DbCountOptions, BackupDbOptions>(args)
                .MapResult(
                    (GpxOptions opts) => RunGpx(opts, exporterService),
                    (LoadOptions opts) => RunLoad(opts, service, log),
                    (VerifyOptions opts) => RunGpxVerify(opts, fileMetaValidator, log),
                    (CsvModifyOptions opts) => RunCsvVerify(opts, log, csvModifierService),
                    (DbCountOptions opts) => RunDbCount(opts, log, csvRepo),
                    (BackupDbOptions opts) => RunBackupDb(opts, log, backupService),
                    errs => 1);
                    
        }

        private static int RunBackupDb(BackupDbOptions opts, Logger log, BackupService backupService)
        {
            log.Information("Using this configuration: {0}", JsonConvert.SerializeObject(opts));

            backupService.BackupDb(opts);

            return 0;
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