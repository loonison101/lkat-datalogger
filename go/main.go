package main

import (
	"./commands"
	jww "github.com/spf13/jwalterweatherman"
	"os"
)

func main() {

	// TODO: Move all this log stuff into it's own file
	jww.SetStdoutThreshold(jww.LevelInfo)
	jww.SetLogThreshold(jww.LevelInfo)

	logFile, err := os.OpenFile("log.txt", os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0644)

	if err != nil {
		jww.ERROR.Println(err)
		panic(err)
	}

	jww.SetLogOutput(logFile)
	//defer


	//runtime.GOMAXPROCS(runtime.NumCPU())
	commands.Execute(os.Args[1:])
	//logFile.Sync()
	//if resp.Err != nil {
	//	if resp.IsUserError() {
	//		resp.Cmd.Println("")
	//		resp.Cmd.Println(resp.Cmd.UsageString())
	//	}
	//	os.Exit(-1)
	//}
}
