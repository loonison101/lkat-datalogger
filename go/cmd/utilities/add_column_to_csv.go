package main

import (
	"bufio"
	"encoding/csv"
	"fmt"
	"github.com/teris-io/shortid"
	"io"
	"log"
	"os"
	"path/filepath"
)

func main() {

	fmt.Println("Loading file...")
	csvFile, err := os.Open("../../../LOG.8.25.18.csv")
	if err != nil {
		log.Panic(err)
	}

	reader := csv.NewReader(bufio.NewReader(csvFile))

	id, err := shortid.Generate()
	if err != nil {
		log.Panic(err)
	}

	destinationFilePath := "../../../generated/" + id + ".csv"
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