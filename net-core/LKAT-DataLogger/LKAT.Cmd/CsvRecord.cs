namespace LKAT.Cmd
{
    public class CsvRecord
    {
        public int Satellites { get; set; }
        public float Hdop { get; set; }
        public float Latitude { get; set; }
        public float Longitude { get; set; }
        public float Age { get; set; }
        public string WhenDate { get; set; }
        public string WhenTime { get; set; }
        public float Altitude { get; set; }
        public string Uuid { get; set; }
        public float Speed { get; set; }
        public string RawLine { get; set; }
        public int BatteryVoltage { get; set; }
    }
}