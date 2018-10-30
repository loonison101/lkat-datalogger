using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using CSharpx;
using Serilog.Core;
using SpatialLite.Core.API;
using SpatialLite.Gps;
using SpatialLite.Gps.Geometries;

namespace LKAT.Cmd
{
    public class CsvExporterService
    {
        private Logger log;

        public CsvExporterService(Logger log)
        {
            this.log = log;
        }

        public void Export()
        {
            log.Information("Beginning export");

            // Load all data from the DB
            using (var db = new CsvDbContext())
            {
                log.Information("Loading all records from DB");
                var records = db.CsvRecords.ToList();
                log.Information("Loaded {0} records", records.Count);

                //var file = new GpxFile();
                
                //file.Tracks = new List<GpxTrack>()
                //{
                //    new GpxTrack()
                //    {
                        
                //    }
                //};

                //GpxWriter.

                //var doc = new GpxDocument(null, null, new List<GpxTrack>()
                //{
                //    new GpxTrack()
                //    {
                //        new GpxTrackSegment()
                //        {
                //            Points = 
                //        }
                //    }
                //})

                // Find unique days
                var days = records.Select(x => x.When.Date).Distinct();
                var days2 = records.GroupBy(x => x.When.Date).OrderBy(x => x.Key.Date).ToList();

               
string dateFormat = "MM-dd-yyyy";
                foreach (IGrouping<DateTime, DbCsvRecord> grouping in days2)
                {
                    var key = grouping.Key;
                    var fileName = Path.Join(Path.GetTempPath(), "GPX-" + key.ToString(dateFormat) + ".gpx"); //Path.GetTempFileName();// + "-gpx-file";
                    log.Information("Beginning creation of GPX file: {0}", fileName);
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
                    

                    log.Information("Added {0} gpx points for this day", points.Count);

                    
                    //var doc = new GpxDocument(points, new List<GpxRoute>(), new List<GpxTrack>());
                    var doc = new GpxDocument(new List<GpxPoint>(), new List<GpxRoute>(), tracks);
                    doc.Save(fileName);





                }

                //days2.First().Key.ToString("MM-dd-yyyy")
                //var points = new List<GpxPoint>();

                //records.ForEach(x =>
                //{
                //    points.Add(new GpxPoint()
                //    {
                //        Metadata = new GpxPointMetadata()
                //        {
                //            AgeOfDgpsData = x.Age,
                //            SatellitesCount = x.Satellites,
                //            Hdop = x.Hdop,

                //        },
                //        Position = new Coordinate(x.Latitude, x.Longitude),
                //        Timestamp = x.When
                //    });
                //});

                //var doc = new GpxDocument(null, null, new List<GpxTrack>(new List<GpxTrack>()));

                //var segment = new GpxTrackSegment(points);
                //var segments = new List<GpxTrackSegment>()
                //{
                //    segment
                //};

                //var track = new GpxTrack(segments);
                ////var doc = new GpxDocument(new List<GpxPoint>(), new List<GpxRoute>(), new List<GpxTrack>()
                ////{
                ////    track
                ////});
                //var doc = new GpxDocument(points, new List<GpxRoute>(), new List<GpxTrack>()); 

                //var fileName = Path.GetTempFileName() + "-gpx-file";
                ////doc.Save(fileName);
                //return fileName;
                log.Information("DONE exporting");
            }

        }
    }
}
