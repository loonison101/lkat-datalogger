package main

import (
	"bytes"
	"flag"
	"fmt"
	"github.com/kniren/gota/dataframe"
	"io/ioutil"
	"log"
)

func main() {

	var csvFile string

	//flag.StringVar(&port, "port", "8080", "The port of the http server")
	flag.StringVar(&csvFile, "CSV File", "./LOG.8.25.18.csv", "The source CSV file path to process")
	flag.Parse()

	log.Output(2, "Beginning LKAT DATALOGGER file processing...")

	//log.Output(2, "CMD Arguments")
	//
	//log.Output(2, flag.Args())

	//rawCsvData, err := ioutil.ReadFile("./data.csv")
	//if err != nil {
	//	//http.Error(response, err.Error(), http.StatusInternalServerError)
	//	log.Panic(err)
	//}

	////dataframe.ReadCSV(ioutil.ReadAll("./data.csv"))
	csvBytes, err := ioutil.ReadFile(csvFile)
	if err != nil {
		log.Panic(err)
	}

	df := dataframe.ReadCSV(bytes.NewReader(csvBytes))

	fmt.Println(df)

}
