
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

#### Setup for a MAC
**Bugs**
* The text file needs a new line \n because it logged on my header column

Put your log file on minio
had to curl -O to download the file
had to update schema 
dotnet ef database update


=============================

### Timeline
**CLI Application**
1) Built parsing application in GoLang. Didn't care for looking at it
2) Built parsing application in .NET core. Looked better but not as easy to use as I woudl like
3) I'm either going to improve the .NET version or write in JavaScript

**Embedded Application**
1) Tried Circut Python but wasn't compatible or I had issues
2) Installed micropython, but certain libraries didn't work
3) Ended up with typical c++ since I know it would work with libraries from Adafruit

### Project Goals
**Embedded Phases**

* Phase 1 - Log GPS coordinates to CSV and manually transfer SD card to get data
* Phase 2 - Upload data to the cloud
* Phase 3 - Make parsing appication easier to use


### Embedded Setup
* `git clone https://github.com/lanekatris/lkat-datalogger`
* cd lkat-datalogger
* cp src/secrets.h.example src/secrets.h
* Fill out `src/secrets.h` with your data with these pictures on setting up S3 and stuff
* build by pressing this image