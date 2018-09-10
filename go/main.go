package main

import (
	"./commands"
	"os"
	jww "github.com/spf13/jwalterweatherman"
)

func main() {

	jww.SetStdoutThreshold(jww.LevelInfo)

	//runtime.GOMAXPROCS(runtime.NumCPU())
	commands.Execute(os.Args[1:])

	//if resp.Err != nil {
	//	if resp.IsUserError() {
	//		resp.Cmd.Println("")
	//		resp.Cmd.Println(resp.Cmd.UsageString())
	//	}
	//	os.Exit(-1)
	//}
}
