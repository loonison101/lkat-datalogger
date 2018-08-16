package main

import (
	"log"
	"io/ioutil"
	"net/http"
	"fmt"
	"encoding/json"
	"encoding/csv"
	"bytes"
	"time"
	"strconv"
	"io"
	"strings"
	"github.com/tkrajina/gpxgo/gpx"
)

func main() {
	log.Output(2, "Beginning LKAT DATALOGGER file processing...");

	log.Output(2, "Route / gives all files in current directory")
	http.HandleFunc("/", getAllFilesHandler)

	log.Output(2, "Sucks in a CSV and dumps a GPX")
	http.HandleFunc("/begin", createGpxHandler)

	log.Output(2, "/testies is used for testing")
	http.HandleFunc("/testies", testiesHandler)

	var port = 8080

	log.Output(2, "HTTP listening on " + string(port) )
	log.Fatal(http.ListenAndServe(":8080", nil))
}

func getAllFilesHandler(response http.ResponseWriter, request *http.Request) {
	// Let's read all files available in folder
	files, err := ioutil.ReadDir(".")
	if err != nil {
		log.Fatal(err)
		http.Error(response, err.Error(), http.StatusInternalServerError)
		return
	}

	var toReturn []File

	for _, file := range files {
		fmt.Printf("file is %v", file.Name())
		toReturn = append(toReturn, File {
			Name: file.Name(),
		})
	}

	json, err := json.Marshal(toReturn)
	if err != nil {
		http.Error(response, err.Error(), http.StatusInternalServerError)
		return
	}

	response.Header().Set("Content-Type", "application/json")
	response.Write(json)
}

type File struct {
	Name string
}

func createGpxHandler(response http.ResponseWriter, request *http.Request) {

	// Load the CSV data
	rawCsvData, err := ioutil.ReadFile("./data.csv")
	if err != nil {
		http.Error(response, err.Error(), http.StatusInternalServerError)
		return
	}

	csvReader := csv.NewReader(bytes.NewReader(rawCsvData))

	var parsedLines []CsvLine

	for {
		line, error := csvReader.Read()
		if error == io.EOF {
			break
		} else if error != nil {
			log.Fatal(error)
		}

		satellites, _ := strconv.ParseInt(line[0], 0, 64)
		hdop, _ := strconv.ParseFloat(line[1], 64)
		latitude, _ := strconv.ParseFloat(line[2],  64)
		longitude, _ := strconv.ParseFloat(strings.Trim(line[3], " "), 64)

		age, _ := strconv.ParseInt(line[4], 0,64)

		month,_ := strconv.ParseInt(strings.Split(line[5], "/")[0], 0, 4)
		day,_ := strconv.ParseInt(strings.Split(line[5], "/")[1], 0, 4)
		year,_ := strconv.ParseInt(strings.Split(line[5], "/")[2], 0, 4)

		hour,_ := strconv.ParseInt(strings.Split(line[6], ":")[0], 0, 4)
		minute,_ := strconv.ParseInt(strings.Split(line[6], ":")[1], 0, 4)
		second,_ := strconv.ParseInt(strings.Split(line[6], ":")[2], 0, 4)

		altitude,_ := strconv.ParseFloat(line[7], 64)
		speed,_ := strconv.ParseFloat(line[8], 64)

		location, _ := time.LoadLocation("EST")

		var newLine = CsvLine{
			Satellites: satellites,
			Hdop: hdop,
			Latitude: latitude,
			Longitude: longitude,
			Age: age,
			When: time.Date(int(year), time.Month(month), int(day), int(hour), int(minute), int(second), 0, location),
			Altitude: altitude,
			Speed: speed,
		}

		parsedLines = append(parsedLines, newLine)
	}

	newFile := gpx.GPX{
		AuthorName: "Lane Katris",
	}

	track := gpx.GPXTrack{}

	segment := gpx.GPXTrackSegment{}

	for _, csvLine := range parsedLines {
		elevation2 := new(gpx.NullableFloat64)
		elevation2.SetValue(csvLine.Altitude)

		segment.Points = append(segment.Points, gpx.GPXPoint{
			Point: gpx.Point{
				Latitude: csvLine.Latitude,
				Longitude: csvLine.Longitude,
				//Elevation: gpx.NullableFloat64{
				//	csvLine.Altitude, true,
				//},
				//Elevation: gpx.NullableFloat64
				//Elevation: elevation2,
			},
			Timestamp: csvLine.When,
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

	fileBytes, err := newFile.ToXml(gpx.ToXmlParams{Version:"1.1", Indent: true})
	if err != nil {
		http.Error(response, err.Error(), http.StatusInternalServerError)
		return
	}


	ioutil.WriteFile("myfirstgpsfile.gpx", fileBytes, 0644)

	//json, err := json.Marshal(newFile)
	//if err != nil {
	//	//log.Fatal(err)
	//	http.Error(response, err.Error(), http.StatusInternalServerError)
	//	return
	//}
	//
	//response.Header().Set("Content-Type", "application/json")
	//response.Write(json)
	response.Write([]byte("done!"))
}

func testiesHandler(response http.ResponseWriter, request *http.Request) {
	//idk, err := strconv.ParseFloat(" -81.546932", 64)
	//
	//if err != nil {
	//	http.Error(response, err.Error(), http.StatusInternalServerError)
	//	return
	//}
	//
	//json, err := json.Marshal(idk)
	//response.Header().Set("Content-Type", "application/json")
	//response.Write(json)

	//newFile := gpx.GPX{
	//	AuthorName: "Lane Katris",
	//}

	//append(newFile.Tracks, )
}

type CsvLine struct {
	Satellites int64
	Hdop float64
	Latitude float64
	Longitude float64
	Age int64
	When time.Time
	Altitude float64
	Speed float64
}