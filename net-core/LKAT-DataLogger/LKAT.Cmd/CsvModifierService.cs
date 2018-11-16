using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using Serilog.Core;

namespace LKAT.Cmd
{
    
    public class CsvModifierService
    {
        private readonly Logger _logger;
        public const string CsvHeader = "Satellites,Hdop,Latitude,Longitude,Age,WhenDate,WhenTime,Altitude,Speed,Uuid,BatteryVoltage";
        

        public CsvModifierService(Logger logger)
        {
            _logger = logger;
        }

        public void AddHeaderToFiles(CsvModifyOptions csvModifyOptions)
        {
            // With no config passed in, let's just find files and let them pass a param to actually do it

            // Search removable drives
            var removableDrives = DriveInfo.GetDrives().Where(x => x.DriveType == DriveType.Removable).ToList();
            var possibleFilesToChange = new List<string>();
            var alreadyHaveHeaderFiles = new List<string>();
            var removableDrivesWithNoLog = new List<string>();

            foreach (var drive in removableDrives)
            {
                IEnumerable<string> logs = Directory.EnumerateFiles(drive.Name, "log.txt");

                if (!logs.Any())
                {
                    removableDrivesWithNoLog.Add(drive.Name);
                }

                // No log file found on removable drive
                if (!logs.Any() && csvModifyOptions.ShouldCreate)
                {
                    string newPath = Path.Join(drive.Name, "LOG.TXT");
                    _logger.Information("Creating file: {0} with headers", newPath);
                    File.WriteAllLines(newPath, new List<string>()
                    {
                        CsvHeader
                    });
                    _logger.Information("{0} created with header", newPath);
                }

                foreach (var fileName in logs)
                {

                    using (StreamReader reader = new StreamReader(fileName))
                    {
                        var firstLine = reader.ReadLine();

                        if (firstLine == CsvHeader)
                        {
                            alreadyHaveHeaderFiles.Add(fileName);
                        }
                        else
                        {
                            
                            possibleFilesToChange.Add(fileName);
                        }
                    }
                }

                
            }

            _logger.Information("These {0} removable drives do NOT have a LOG.TXT file", removableDrivesWithNoLog.Count);
            removableDrivesWithNoLog.ForEach(x => _logger.Information("{0}", x));

            _logger.Information("These {0} files could have the header added:", possibleFilesToChange.Count);
            possibleFilesToChange.ForEach(x => _logger.Information("{0}", x));

            _logger.Information("These {0} files do NOT need header:", alreadyHaveHeaderFiles.Count);
            alreadyHaveHeaderFiles.ForEach(x => _logger.Information("{0}", x));

            if (!csvModifyOptions.ShouldWrite)
            {
                _logger.Warning("ShouldWrite was false, not modifying any files");
                return;
            }

            
            foreach (var fileToChange in possibleFilesToChange)
            {
                _logger.Information("Modifying header in file: {0}", fileToChange);
                string tempFile = Path.GetTempFileName();
                using (var writer = new StreamWriter(tempFile))
                using (var reader = new StreamReader(fileToChange))
                {
                    // Add the header
                    writer.WriteLine(CsvHeader);

                    while (!reader.EndOfStream)
                        writer.WriteLine(reader.ReadLine());
                }

                // Now copy over the contents
                File.Copy(tempFile, fileToChange, true);

                // No need for the temp file
                File.Delete(tempFile);

                _logger.Information("Done modifying file: {0}", fileToChange);
            }
        }
    }
}