using System;
using System.Collections.Generic;
using CommandLine;
using Newtonsoft.Json;
using Serilog;

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
            
            return CommandLine.Parser.Default.ParseArguments<GpxOptions, LoadOptions>(args)
                .MapResult(
                    (GpxOptions opts) => RunGpx(opts, exporterService),
                    (LoadOptions opts) => RunLoad(opts, service, log),
                    errs => 1);
                    
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