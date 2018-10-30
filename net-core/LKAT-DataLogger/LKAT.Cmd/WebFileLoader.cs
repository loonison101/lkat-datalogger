using System.Net;
using Serilog.Core;

namespace LKAT.Cmd
{
    public class WebFileLoader : IFileLoader
    {
        private readonly Logger _logger;

        public WebFileLoader(Logger logger)
        {
            _logger = logger;
        }

        public bool ShouldLoad(string sourceFilePath)
        {
            return sourceFilePath.ToLower().Contains("http");
        }

        public void Load(string sourceFilePath, string destinationFileName)
        {
            var client = new WebClient();
            
            _logger.Information("Downloading file: " + sourceFilePath);
            client.DownloadFile(sourceFilePath, destinationFileName);
            _logger.Information("Downloaded file: " + destinationFileName);
        }
    }
}