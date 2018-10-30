using System.IO;
using Serilog.Core;

namespace LKAT.Cmd
{
    public class LocalFileLoader : IFileLoader
    {
        private readonly Logger _logger;

        public LocalFileLoader(Logger logger)
        {
            _logger = logger;
        }

        public bool ShouldLoad(string sourceFilePath)
        {
            return !sourceFilePath.ToLower().Contains("http");
        }

        public void Load(string sourceFilePath, string destinationFileName)
        {
            // No need to do anything
            //File.Copy(sourceFilePath, destinationFileName);
            
            _logger.Information("Copying {0} to {1}", sourceFilePath, destinationFileName);
            using (var sourceStream = new FileStream(sourceFilePath, FileMode.Open))
            using (var destinationStream = new FileStream(destinationFileName, FileMode.OpenOrCreate))
            {
                sourceStream.CopyTo(destinationStream);
            }
        }
    }
}