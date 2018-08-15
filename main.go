package main

import (
	"log"
	"io/ioutil"
	"net/http"

	"fmt"
	"html/template"
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


	// If the CSV file exists, load csv,


	// Do we have a file to process?
	//p1 := &Page{Title: "TestPage", Body: []byte("This is a sample Page.")}
	//p1.save()
	//p2, _ := loadPage("TestPage")
	//fmt.Println(string(p2.Body))

	//http.HandleFunc("/", handler)
	//http.HandleFunc("/view/", viewHandler)
	//http.HandleFunc("/edit/", editHandler)
	//http.HandleFunc("/save/", saveHandler)
	http.HandleFunc("/", getAllFilesHandler)
	http.HandleFunc("/begin", createGpxHandler)
	http.HandleFunc("/testies", testiesHandler)

	port := 8080

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

func viewHandler(w http.ResponseWriter, r *http.Request) {
	title := r.URL.Path[len("/view/"):]
	p, _ := loadPage(title)
	renderTemplate(w, "view", p)
}
func handler(w http.ResponseWriter, r *http.Request) {
	fmt.Fprintf(w, "Hi there, I love %s!", r.URL.Path[1:])
}

func (p *Page) save() error {
	filename := p.Title + ".txt"
	return ioutil.WriteFile(filename, p.Body, 0600)
}

func loadPage(title string) (*Page, error) {
	filename := title + ".txt"
	body, err := ioutil.ReadFile(filename)
	if err != nil {
		return nil, err
	}
	return &Page{Title: title, Body: body}, nil
}

func editHandler(w http.ResponseWriter, r *http.Request) {
	title := r.URL.Path[len("/edit/"):]
	p, err := loadPage(title)
	if err != nil {
		p = &Page{Title: title}
	}
	renderTemplate(w, "edit", p)
}
func renderTemplate(w http.ResponseWriter, tmpl string, p *Page) {
	t, _ := template.ParseFiles(tmpl + ".html")
	t.Execute(w, p)
}

type File struct {
	Name string
}

type Page struct {
	Title string
	Body  []byte
}

func createGpxHandler(response http.ResponseWriter, request *http.Request) {
	// Load the CSV data
	rawCsvData, err := ioutil.ReadFile("./data.csv")
	if err != nil {
		//log.Fatal(err)
		http.Error(response, err.Error(), http.StatusInternalServerError)
		return
	}

	csvReader := csv.NewReader(bytes.NewReader(rawCsvData))

	//csvLines, err := csvReader.ReadAll()
	//if err != nil {
	//	//log.Fatal(err)
	//	http.Error(response, err.Error(), http.StatusInternalServerError)
	//	return
	//}

	var parsedLines []CsvLine

	//for _, line := range csvLines {
	//	var a = line[0]
	//
	//
	//	var lineToAdd = strconv.ParseInt(a, 0, 32)
	//
	//	parsedLines = append(parsedLines, CsvLine{
	//		Satellites: strconv.ParseInt(line[0], 0, 32)
	//	})
	//}

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