package main

import (
	"bytes"
	"encoding/csv"
	"flag"
	"fmt"
	"io"
	"strconv"
	"strings"
	"time"

	//"github.com/derekparker/delve/pkg/dwarf/reader"

	"../../pkg"
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
	//csvBytes, err := ioutil.ReadFile(csvFile)
	//if err != nil {
	//	log.Panic(err)
	//}

	//df := dataframe.ReadCSV(bytes.NewReader(csvBytes))

	//fmt.Println(df)

	// Let's create our structs
	//var parsedLines [] pkg.CsvLine

	//sel1 := df.Select([]int{0,1})
	//
	//fmt.Println(sel1)

	//var reader csv.Reader

	var reader = getCsvReader(csvFile)
	csvLines := createCsvLines(reader)

	//fmt.Println(csvLines)
	fmt.Println(len(csvLines))
	lastLine := csvLines[len(csvLines)-1]
	fmt.Println(lastLine)
}

func getCsvReader(csvFile string) (reader *csv.Reader) {
	// Load the CSV data
	rawCsvData, err := ioutil.ReadFile(csvFile)
	if err != nil {
		log.Panic(err)
	}

	return csv.NewReader(bytes.NewReader(rawCsvData))
}

func createCsvLines(reader *csv.Reader) (parsedLines []pkg.CsvLine) {
	//var parsedLines []pkg.CsvLine

	//var index int = 0

	for {
		line, error := reader.Read()
		if error == io.EOF {
			break
		} else if error != nil {
			log.Fatal(error)
		}

		satellites, _ := strconv.ParseInt(line[0], 0, 64)
		hdop, _ := strconv.ParseFloat(line[1], 64)
		latitude, _ := strconv.ParseFloat(line[2], 64)
		longitude, _ := strconv.ParseFloat(strings.Trim(line[3], " "), 64)

		age, _ := strconv.ParseInt(line[4], 0, 64)

		//month,_ := strconv.ParseInt(strings.Split(line[5], "/")[0], 0, 64)
		month, _ := strconv.Atoi(strings.Split(line[5], "/")[0]) //(int)strings.Split(line[5], "/")[0]
		//day,_ := strconv.ParseInt(strings.Split(line[5], "/")[1], 0, 64)
		day, _ := strconv.Atoi(strings.Split(line[5], "/")[1])
		//year,_ := strconv.ParseInt(strings.Split(line[5], "/")[2], 0, 64)
		year, _ := strconv.Atoi(strings.Split(line[5], "/")[2])

		hour, _ := strconv.Atoi(strings.Split(line[6], ":")[0])

		minute, _ := strconv.Atoi(strings.Split(line[6], ":")[1])
		second, _ := strconv.Atoi(strings.Split(line[6], ":")[2])

		altitude, _ := strconv.ParseFloat(line[7], 64)
		speed, _ := strconv.ParseFloat(line[8], 64)

		location, _ := time.LoadLocation("GMT")

		//dateman := fmt.Sprintf("%d-%d-%d %s", year, month, day, line[6])
		//dateFormat := "2006-01-02 14:04:05"

		//ughDate, err := time.Parse(dateman, dateFormat)
		//if err != nil {
		//	http.Error(response, err.Error(), http.StatusInternalServerError)
		//	return
		//}

		daDate := time.Date(int(year), time.Month(month), int(day), int(hour), int(minute), int(second), 0, location)

		var newLine = pkg.CsvLine{
			Satellites: satellites,
			Hdop:       hdop,
			Latitude:   latitude,
			Longitude:  longitude,
			Age:        age,
			When:       daDate,
			//When: ughDate,
			Altitude: altitude,
			Speed:    speed,
			RawLine:  strings.Join(line, ","),
		}

		parsedLines = append(parsedLines, newLine)

	}

	return parsedLines
}
