package commands

import (
	"github.com/spf13/cobra"
	"log"
)

//var _ cmder = (*versionCmd)(nil)
//
//type versionCmd struct {
//	*baseCmd
//}

//func csvCmd() *versionCmd {
//	return &versionCmd{
//		newBaseCmd(&cobra.Command{
//			Use: "csv",
//			Run: func(cmd *cobra.Command, args []string) {
//				log.Print(2, "csv command lets go!!")
//			},
//		}),
//	}
//}

type CsvCommand struct {}

func (c CsvCommand) GetCommand(args []string) *cobra.Command {
	return &cobra.Command{
		Use: "csv",
		Run: func(cmd *cobra.Command, args2 []string) {
			log.Print(2, "csv command lets go")
		},
	}
}