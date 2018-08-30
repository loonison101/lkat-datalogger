package main

import (
	"../../pkg"
	"bytes"
	"encoding/csv"
	"flag"
	"fmt"
	"github.com/teris-io/shortid"
	"github.com/tkrajina/gpxgo/gpx"
	"io"
	"io/ioutil"
	"log"
	"os"
	"path/filepath"
	"strconv"
	"strings"
	"time"
)

type GpxSplitOptions int

const (
	NONE GpxSplitOptions = 1 + iota
	DAY
)

func main() {

	var csvFile string
	var destinationFile string
	defaultDestinationFile, _ := shortid.Generate()

	flag.StringVar(&csvFile, "CSV File", "../../../LOG.8.25.18.csv", "The source CSV file path to process")
	flag.StringVar(&destinationFile, "Destination GPX file", "../../../generated/"+defaultDestinationFile+".gpx", "The destination gpx file made from the csv data")
	flag.Parse()

	log.Output(2, "Beginning LKAT DATALOGGER file processing...")

	var reader = getCsvReader(csvFile)
	csvLines := createCsvLines(reader)

	log.Output(2, fmt.Sprintf("Loaded %d csv lines", len(csvLines)))

	// Create the gpx object
	gpxObject := createGpx(csvLines)

	writeGpxFile(gpxObject, destinationFile)

	absoluteDestinationFile, _ := filepath.Abs(destinationFile)
	log.Output(2, fmt.Sprintf("Done writing gpx file to %s", absoluteDestinationFile))

}
func writeGpxFile(gpxObject gpx.GPX, destinationFile string) {
	fileBytes, err := gpxObject.ToXml(gpx.ToXmlParams{Version: "1.1", Indent: true})
	if err != nil {
		log.Panic(err)
	}

	err = os.MkdirAll(filepath.Dir(destinationFile), os.ModePerm)
	if err != nil {
		log.Panic(err)
	}
	err = ioutil.WriteFile(destinationFile, fileBytes, 0644)

	if err != nil {
		log.Panic(err)
	}
}

func createGpx(csvLines []pkg.CsvLine) gpx.GPX {
	newFile := gpx.GPX{
		AuthorName: "Lane Katris",
	}

	track := gpx.GPXTrack{}

	segment := gpx.GPXTrackSegment{}

	for _, csvLine := range csvLines {
		//elevation2 := new(gpx.NullableFloat64)
		//elevation2.SetValue(csvLine.Altitude)

		elevation3 := gpx.NullableFloat64{}
		elevation3.SetValue(csvLine.Altitude)

		satellites := gpx.NullableInt{}
		satellites.SetValue(csvLine.Satellites)

		age := gpx.NullableFloat64{}
		age.SetValue(csvLine.Age)

		segment.Points = append(segment.Points, gpx.GPXPoint{
			Point: gpx.Point{
				Latitude:  csvLine.Latitude,
				Longitude: csvLine.Longitude,
				Elevation: elevation3,
			},
			Timestamp:     csvLine.When,
			Source:        "Go Pro",
			Satellites:    satellites,
			AgeOfDGpsData: age,
			//Satellites: csvLine.Satellites,
			//Satellites: gpx.NullableInt{
			//	int(csvLine.Satellites), true,
			//},
			////AgeOfDGpsData: csvLine.Age
			//AgeOfDGpsData: gpx.NullableFloat64{
			//	float64(csvLine.Age), true,
			//},
		})
	}

	track.Segments = append(track.Segments, segment)

	newFile.Tracks = append(newFile.Tracks, track)

	return newFile
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

	for {
		line, error := reader.Read()
		if error == io.EOF {
			break
		} else if error != nil {
			log.Fatal(error)
		}

		satellites, _ := strconv.Atoi(line[0]) //strconv.ParseInt(line[0], 0, 64)
		hdop, _ := strconv.ParseFloat(line[1], 64)
		latitude, _ := strconv.ParseFloat(line[2], 64)
		longitude, _ := strconv.ParseFloat(strings.Trim(line[3], " "), 64)

		//age, _ := strconv.ParseInt(line[4], 0, 64)
		age, _ := strconv.ParseFloat(line[4], 64)

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

		// altitude is in meters, let's convert to feet
		altitude = altitude * 3.28084

		speed, _ := strconv.ParseFloat(line[8], 64)

		location, _ := time.LoadLocation("GMT")

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
