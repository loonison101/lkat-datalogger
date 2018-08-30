package main

import (
	"bytes"
	"encoding/csv"
	"encoding/json"
	"flag"
	"fmt"
	"github.com/tkrajina/gpxgo/gpx"
	"io"
	"io/ioutil"
	"log"
	"net/http"
	"strconv"
	"strings"
	"time"
)

var port string

func main() {
	log.Output(2, "Beginning LKAT DATALOGGER file processing...")

	flag.StringVar(&port, "port", "8080", "The port of the http server")

	flag.Parse()

	log.Output(2, "Route / gives all files in current directory")
	http.HandleFunc("/", getAllFilesHandler)

	log.Output(2, "Sucks in a CSV and dumps a GPX")
	http.HandleFunc("/begin", createGpxHandler)

	log.Output(2, "/testies is used for testing")
	http.HandleFunc("/testies", testiesHandler)

	log.Output(2, "HTTP listening on "+port)
	log.Fatal(http.ListenAndServe(":"+port, nil))
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
		toReturn = append(toReturn, File{
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

type NullableFloat64 struct {
	data    float64
	notNull bool
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
		latitude, _ := strconv.ParseFloat(line[2], 64)
		longitude, _ := strconv.ParseFloat(strings.Trim(line[3], " "), 64)

		age, _ := strconv.ParseInt(line[4], 0, 64)

		//month,_ := strconv.ParseInt(strings.Split(line[5], "/")[0], 0, 64)
		month, _ := strconv.Atoi(strings.Split(line[5], "/")[0]) //(int)strings.Split(line[5], "/")[0]
		//day,_ := strconv.ParseInt(strings.Split(line[5], "/")[1], 0, 64)
		day, _ := strconv.Atoi(strings.Split(line[5], "/")[1])
		//year,_ := strconv.ParseInt(strings.Split(line[5], "/")[2], 0, 64)
		year, _ := strconv.Atoi(strings.Split(line[5], "/")[2])

		hour, _ := strconv.ParseInt(strings.Split(line[6], ":")[0], 0, 4)

		minute, _ := strconv.ParseInt(strings.Split(line[6], ":")[1], 0, 4)
		second, _ := strconv.ParseInt(strings.Split(line[6], ":")[2], 0, 4)

		altitude, _ := strconv.ParseFloat(line[7], 64)
		speed, _ := strconv.ParseFloat(line[8], 64)

		location, _ := time.LoadLocation("UTC")

		//dateman := fmt.Sprintf("%d-%d-%d %s", year, month, day, line[6])
		//dateFormat := "2006-01-02 14:04:05"

		//ughDate, err := time.Parse(dateman, dateFormat)
		//if err != nil {
		//	http.Error(response, err.Error(), http.StatusInternalServerError)
		//	return
		//}

		daDate := time.Date(int(year), time.Month(month), int(day), int(hour), int(minute), int(second), 0, location)

		var newLine = CsvLine{
			Satellites: satellites,
			Hdop:       hdop,
			Latitude:   latitude,
			Longitude:  longitude,
			Age:        age,
			When:       daDate,
			//When: ughDate,
			Altitude: altitude,
			Speed:    speed,
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
				Latitude:  csvLine.Latitude,
				Longitude: csvLine.Longitude,
				//Elevation: NullableFloat64{
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

	fileBytes, err := newFile.ToXml(gpx.ToXmlParams{Version: "1.1", Indent: true})
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