
### Build Commands
```go build main.go```

# .NET Core
```dotnet publish -f netcoreapp2.1 -c Release```

In bin/Release/netcoreapp2.1
```dotnet LKAT.Cmd.dll```

Verify GPX files with DB
```dotnet .\LKAT.Cmd.dll verify -d "C:\Users\looni\AppData\Local\Temp\gpx"```