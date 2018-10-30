using System;
using System.Collections.Generic;
using System.Text;
using LKAT.Cmd;
using Serilog;
using Serilog.Core;

namespace LKAT.Test
{
    internal class Mocks
    {
        internal static Logger Logger()
        {
            var log = new LoggerConfiguration()
                .MinimumLevel.Debug()
                .WriteTo.Console()
                .WriteTo.File("logs\\log.txt", rollingInterval: RollingInterval.Month)
                .CreateLogger();

            return log;
        }

        internal static List<IFileLoader> Loaders()
        {
            var log = Mocks.Logger();

            var loaders = new List<IFileLoader>()
            {
                new LocalFileLoader(log),
                new WebFileLoader(log)
            };

            return loaders;
        }
    }
}
