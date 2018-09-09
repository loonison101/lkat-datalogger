package commands

import (
	"bufio"
	"encoding/csv"
	"fmt"
	"github.com/spf13/cobra"
	"github.com/teris-io/shortid"
	"io"
	"log"
	"os"
	"path/filepath"
)

type EnrichCsvCommand struct {}

var sourceCsvPath string

func (c EnrichCsvCommand) GetCommand() *cobra.Command {
	cmd := &cobra.Command{
		Use: "enrich",
		Run: doWork,
	}

	cmd.Flags().StringVarP(&sourceCsvPath, "source", "s", "../LOG.8.25.18.csv", "source CSV file to process")

	return cmd
}

func doWork(cmd *cobra.Command, args []string) {
	fmt.Println("Loading file... " + sourceCsvPath )
	csvFile, err := os.Open(sourceCsvPath)
	if err != nil {
		log.Panic(err)
	}

	reader := csv.NewReader(bufio.NewReader(csvFile))

	id, err := shortid.Generate()
	if err != nil {
		log.Panic(err)
	}

	destinationFilePath := "../generated/" + id + ".csv"
	destinationFile, err := os.Create(destinationFilePath)
	if err != nil {
		log.Panic(err)
	}
	defer destinationFile.Close()

	writer := csv.NewWriter(destinationFile)
	defer writer.Flush()

	for {
		line, err := reader.Read()
		if err == io.EOF {
			break
		} else if err != nil {
			log.Panic(err)
		}

		tempFileName, err := shortid.Generate()
		if err != nil {
			log.Panic(err)
		}

		line = append(line, tempFileName)

		err = writer.Write(line)
		if err != nil {
			log.Panic(err)
		}

	}

	absoluteDestination, err := filepath.Abs(destinationFilePath)
	if err != nil {
		log.Panic(err)
	}

	fmt.Println("Created file: " + absoluteDestination)
}