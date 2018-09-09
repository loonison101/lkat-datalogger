package main

import (
	"./commands"
	"os"
)

func main() {

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
