using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using CsvHelper;
using CsvHelper.Configuration;
using CSharpx;
using EFCore.BulkExtensions;
using Microsoft.Extensions.Logging;
using Serilog.Core;

namespace LKAT.Cmd
{
    public class CsvLoaderService
    {
        private Logger _logger;
        private List<IFileLoader> _loaders;


        public CsvLoaderService(Logger logger, List<IFileLoader> loaders)
        {
            _logger = logger;
            _loaders = loaders;
        }

        public void LoadFileIntoDb(LoadOptions opts)
        {
            
            var fileName = Path.GetTempFileName();

            
            var loader = _loaders.First(x => x.ShouldLoad(opts.sourceFile));
            _logger.Information("Using loader: {Name}", loader.GetType().Name);
            loader.Load(opts.sourceFile, fileName);

            _logger.Information("Loading CSV");
            TextReader reader = new StreamReader(fileName);
            var csvReader = new CsvReader(reader, new Configuration()
            {
                //HeaderValidated = null,
                //HeaderValidated = delegate(bool b, string[] strings, int arg3, ReadingContext arg4)
                //{
                //    Console.WriteLine("headervalidated error");
                //},
                //MissingFieldFound = delegate(string[] strings, int i, ReadingContext arg3)
                //{
                //    Console.WriteLine("someting is wrong with missing field found");
                //}
                MissingFieldFound =  null
            });

            List<DbCsvRecord> dbRecords = new List<DbCsvRecord>();

            _logger.Information("Reading CSV file");
            csvReader.Read();
            csvReader.ReadHeader();
            while (csvReader.Read())
            {
                CsvRecord x = csvReader.GetRecord<CsvRecord>();
                
                dbRecords.Add(new DbCsvRecord()
                {
                    Satellites = x.Satellites,
                    Hdop = x.Hdop,
                    Latitude = x.Latitude,
                    Longitude = x.Longitude,
                    Age = x.Age,
                    When = DateTime.Parse(x.WhenDate + " " + x.WhenTime),
                    Altitude = x.Altitude,
                    Uuid = x.Uuid,
                    Speed = x.Speed,
                    RawLine = x.RawLine,
                    BatteryVoltage = x.BatteryVoltage
                });
            }
            _logger.Information("Loaded {0} CSV lines", dbRecords.Count);

            using (var db = new CsvDbContext())
            {
                float i = 1;
                foreach (var record in dbRecords)
                {
                    if (!db.CsvRecords.Any(x => x.Uuid == record.Uuid))
                    {
                        _logger.Information("Going to create this record: {0}", record.Uuid);
                        db.Add(record);
                    }
                    
                    _logger.Information("Processed line - {0}", i / (float)dbRecords.Count);
                    i++;
                }

                db.SaveChanges();
            }

            _logger.Information("NOT Deleting {0}", fileName);
            //File.Delete(fileName);
            
            //_logger.Information("Loaded {0} records", length);
            _logger.Information("DONE");
        }
    }
}