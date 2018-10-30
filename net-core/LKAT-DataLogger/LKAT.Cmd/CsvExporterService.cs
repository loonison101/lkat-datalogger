using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using CSharpx;
using Serilog.Core;
using SpatialLite.Core.API;
using SpatialLite.Gps;
using SpatialLite.Gps.Geometries;

namespace LKAT.Cmd
{
    public class CsvExporterService
    {
        private readonly Logger _logger;

        public CsvExporterService(Logger logger)
        {
            this._logger = logger;
        }

        public void Export()
        {
            _logger.Information("Beginning export");

            // Load all data from the DB
            using (var db = new CsvDbContext())
            {
                _logger.Information("Loading all records from DB");
                var records = db.CsvRecords.ToList();
                _logger.Information("Loaded {0} records", records.Count);

                // Find unique days (no time)
                var groupedDays = records.GroupBy(x => x.When.Date).OrderBy(x => x.Key.Date).ToList();

               
                string dateFormat = "MM-dd-yyyy";
                foreach (IGrouping<DateTime, DbCsvRecord> grouping in groupedDays)
                {
                    var key = grouping.Key;
                    var fileName = Path.Join(Path.GetTempPath(), "GPX-" + key.ToString(dateFormat) + ".gpx"); //Path.GetTempFileName();// + "-gpx-file";
                    _logger.Information("Beginning creation of GPX file: {0}", fileName);
                    var points = new List<GpxPoint>();
                    
                   

                    grouping.OrderBy(x => x.When).ForEach(x =>
                    {
                        points.Add(new GpxPoint()
                        {
                            Metadata = new GpxPointMetadata()
                            {
                                AgeOfDgpsData = x.Age,
                                SatellitesCount = x.Satellites,
                                Hdop = x.Hdop,
                                
                            },
                            Position = new Coordinate(x.Longitude, x.Latitude),
                            Timestamp = x.When,
                            
                        });
                        //gpxTrackSegments.Add(new GpxTrackSegment());
                    });

                    
                    var gpxTrackSegments = new List<GpxTrackSegment>()
                    {
                        new GpxTrackSegment(points)
                    };
                    var track = new GpxTrack(gpxTrackSegments);
                    var tracks = new List<GpxTrack>()
                    {
                        track
                    };

                    _logger.Information("Added {0} gpx points for this day", points.Count);

                    var doc = new GpxDocument(new List<GpxPoint>(), new List<GpxRoute>(), tracks);
                    doc.Save(fileName);
                }

                _logger.Information("DONE exporting");
            }

        }
    }
}
