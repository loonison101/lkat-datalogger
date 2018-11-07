using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace LKAT.Cmd
{
    public class CsvRepository
    {
        public int RecordCount()
        {
            using (var context = new CsvDbContext())
            {
                return context.CsvRecords.Count();
            }
        }
    }
}
