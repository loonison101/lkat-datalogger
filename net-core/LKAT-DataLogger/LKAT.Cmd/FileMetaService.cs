using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Security.Cryptography;
using System.Text;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace LKAT.Cmd
{
    public class FileMetaService
    {
        private readonly string _apiKey;
        private readonly string _dbId;

        public FileMetaService(string apiKey, string dbId)
        {
            if (string.IsNullOrWhiteSpace(apiKey) || string.IsNullOrWhiteSpace(dbId)) throw new ArgumentNullException();
            _apiKey = apiKey;
            _dbId = dbId;
        }

        public List<FileMeta> GetLocal(string directory)
        {
            string[] filePaths = Directory.GetFiles(directory);
            List<FileMeta> files = new List<FileMeta>();

            foreach (var path in filePaths)
            {
                SHA256 cryptoProvider = SHA256.Create();

                using (FileStream stream = File.OpenRead(path))
                {
                    byte[] hashBytes = cryptoProvider.ComputeHash(stream);
                    files.Add(new FileMeta()
                    {
                        NameWithExtension = Path.GetFileName(path),
                        NameWithPath = path,
                        Hash = BytesToString(hashBytes)
                    });
                }
            }

            return files;
        }

        private string BytesToString(byte[] bytes)
        {
            string result = "";
            foreach (byte b in bytes) result += b.ToString("x2");
            return result;
        }

        public List<FileMeta> GetFromDb()
        {
            using (WebClient client = new WebClient())
            {
                client.Headers.Add("Authorization", $"Bearer {this._apiKey}");

                string response = client.DownloadString(
                    $"https://api.airtable.com/v0/{this._dbId}/GpxFile?maxRecords=100&view=Grid%20view");

                JObject parsedResponse = JObject.Parse(response);

                var query = from r in parsedResponse["records"]
                            select (JObject)r["fields"];

                var files = new List<FileMeta>();
                foreach (var entry in query)
                {
                    files.Add(new FileMeta()
                    {
                        NameWithExtension = (string)entry["NameWithExtension"],
                        NameWithPath = (string)entry["NameWithPath"],
                        Hash = (string)entry["Hash"],
                        Status = entry["Status"] == null ? null :  JArray.Parse(entry["Status"].ToString()).ToString()//(string[]) entry["Status"]
                    });
                }

                return files;
            }
        }

        public void InsertFilesToDb(List<FileMeta> filesToCreate)
        {
            foreach (var file in filesToCreate)
            {
                string body = JsonConvert.SerializeObject(new
                {
                    fields = new
                    {
                        NameWithExtension = file.NameWithExtension,
                        Hash = file.Hash,
                        NameWithPath = file.NameWithPath
                    }
                });

                using (WebClient client = new WebClient())
                {
                    client.Headers.Add("Authorization", $"Bearer {this._apiKey}");
                    client.Headers.Add("Content-type", "application/json");

                    client.UploadString($"https://api.airtable.com/v0/{this._dbId}/GpxFile", "POST", body);
                }
            }
        }
    }
}
