namespace LKAT.Cmd
{
    public interface IFileLoader
    {
        bool ShouldLoad(string sourceFilePath);
        void Load(string sourceFilePath, string destinationFileName);
    }
}