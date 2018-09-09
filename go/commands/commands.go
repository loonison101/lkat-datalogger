package commands

import "github.com/spf13/cobra"

type ICommand interface {
	GetCommand(args []string) cobra.Command
}

func Execute(args []string) {
	// this is where we bring together all of our hugo stuff

	var rootCmdLkat = &cobra.Command{Use: "app"}

	csvCommand := AllCommand{}
	rootCmdLkat.AddCommand(csvCommand.GetCommand(args))

	enrichCsvCommand := EnrichCsvCommand{}
	rootCmdLkat.AddCommand(enrichCsvCommand.GetCommand())

	importCommand := ImportCsvToDbCommand{}
	rootCmdLkat.AddCommand(importCommand.GetCommand(args))

	rootCmdLkat.Execute()
}
//type baseCmd struct {
//	cmd *cobra.Command
//}
//
//func newBaseCmd(cmd *cobra.Command) *baseCmd {
//	return &baseCmd{cmd: cmd}
//}
//
//type baseBuilderCmd struct {
//	*baseCmd
//	*commandsBuilder
//}
//func newCommandsBuilder() *commandsBuilder {
//	return &commandsBuilder{}
//}
//func (b *commandsBuilder) addCommands(commands ...cmder) *commandsBuilder {
//	b.commands = append(b.commands, commands...)
//	return b
//}
//func (b *commandsBuilder) addAll() *commandsBuilder {
//	b.addCommands(
//		csvCmd(),
//	)
//
//	return b
//}
//
//var _ cmder = (*nilCommand)(nil)
//
//type nilCommand struct {
//}
//
//type commandsBuilder struct {
//	builderData
//
//	commands []cmder
//}
//
//type builderData struct {
//	logging bool
//}
//
//var _ commandsBuilderGetter = (*baseBuilderCmd)(nil)