
### Build Commands
```go build main.go```

# .NET Core
```dotnet publish -f netcoreapp2.1 -c Release```

In bin/Release/netcoreapp2.1
```dotnet LKAT.Cmd.dll```

Verify GPX files with DB
```dotnet .\LKAT.Cmd.dll verify -d "C:\Users\looni\AppData\Local\Temp\gpx"```

Find how many records in DB

```dotnet .LKAT.Cmd.dll db-count```

## Syncing new data from embedded SD card
Plugin in SD card (obviously :smile: )

Make sure the file is in the correct format

```dotnet .\LKAT.Cmd.dll verify-csv -y```

Load the CSV file into the DB (file or web)

```dotnet .\LKAT.Cmd.dll load -s F:\log.txt```

```dotnet .\LKAT.Cmd.dll load -s https://storage.googleapis.com/YOUR_BIN_HERE/LOG-11-05-18.TXT```


Generate the GPX files from the DB (will overwrite)

```dotnet .\LKAT.Cmd.dll gpx```

Verify GPX files with meta DB and see what files haven't been added to socially shared

```dotnet .\LKAT.Cmd.dll verify-gpx -d C:\Users\looni\AppData\Local\Temp\gpx```

Backup DB

```dotnet .\LKAT.Cmd.dll backup-db```