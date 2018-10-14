package commands

import (
	"bufio"
	"bytes"
	"encoding/csv"
	"fmt"
	"github.com/jinzhu/gorm"
	_ "github.com/jinzhu/gorm/dialects/sqlite"
	"github.com/spf13/cobra"
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
	"../pkg"
)


type AllCommand struct {}

func (c AllCommand) GetCommand(args []string) *cobra.Command {

	var populateDb bool
	var populateGpx bool
	var csvFile string
	var destinationFile string
	defaultDestinationFile, _ := shortid.Generate()

	cmd := &cobra.Command{
		Use: "all",
		Run: func(cmd *cobra.Command, args2 []string) {
			//log.Print(2, "csv command lets go")


			log.Output(2, "Beginning LKAT DATALOGGER file processing...")

			var reader = getCsvReader(csvFile)
			csvLines := createCsvLines(reader)

			log.Output(2, fmt.Sprintf("Loaded %d csv lines", len(csvLines)))

			// We also need to populate a central store that will contain all of our data
			// Maybe do a big select on IDs...
			//if true {
			db, err := gorm.Open("sqlite3", "lkat.db")
			if err != nil {
				panic("failed to connect database - " + err.Error())
			}
			defer db.Close()

			// Migrate the schema
			db.AutoMigrate(&pkg.CsvLine{})

			// I'll handle errors
			db.LogMode(false)

			//}

			if populateGpx {
				log.Output(2, "Creating GPX file...")
				// Create the gpx object
				gpxObject := createGpx(csvLines)

				writeGpxFile(gpxObject, destinationFile)
			} else {
				log.Output(2, "Not creating GPX file because populateGpx is false")
			}


			// Load database
			if populateDb {
				log.Output(2, "Beginning populating DB since populateDb is true")
				loadDatabase(csvLines, db)
			} else {
				log.Output(2, "Not populating DB since populateDb is false")
			}


			absoluteDestinationFile, _ := filepath.Abs(destinationFile)
			log.Output(2, fmt.Sprintf("Done writing gpx file to %s", absoluteDestinationFile))

			fmt.Print("Press 'Enter' to close program...")
			bufio.NewReader(os.Stdin).ReadBytes('\n')




		},
	}

	cmd.Flags().StringVarP(&csvFile, "source", "s", "../LOG.8.25.18.csv", "The source CSV file path to process")
	cmd.Flags().StringVarP(&destinationFile, "destination", "d", "../generated/"+defaultDestinationFile+".gpx", "The destination gpx file made from the csv data")
	cmd.Flags().BoolVarP(&populateDb, "populateDb", "p", true, "Defaults to true which populates the DB, false if you do not want to populate it")
	cmd.Flags().BoolVarP(&populateGpx, "populateGpx", "g", true, "Defaults to true which populates the GPX file, false if you do not want to create the file")

	return cmd
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

	// gpx file -> tracks -> segments
	track := gpx.GPXTrack{}


	// We need to generate se

	segment := gpx.GPXTrackSegment{}

	for _, csvLine := range csvLines {

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
		})
	}

	track.Segments = append(track.Segments, segment)

	newFile.Tracks = append(newFile.Tracks, track)

	return newFile
}

func loadDatabase(csvLines []pkg.CsvLine, db *gorm.DB) {


	// We don't want to insert duplicates into our DB, so what to do...
	//
	tx := db.Begin()
	var idx float32 = 1;
	var len = float32(len(csvLines))
	for _, line := range csvLines {
		var diff float32 = idx / len
		log.Output(2, fmt.Sprintf("Creating record in db, index: %f, %f%% done ", idx, diff))
		if err := tx.Create(&line).Error; err != nil {
			if strings.Contains(err.Error(), "UNIQUE constraint failed: csv_lines.uuid") {
				// It's ok, we are aware of this error, we don't want duplicates anyway
			} else {
				log.Fatal(err)
			}
		}
		idx = idx + 1
	}

	fmt.Print("Ready to commit to DB? Press 'Enter' to continue...")
	bufio.NewReader(os.Stdin).ReadBytes('\n')

	log.Output(2, "Commiting transaction...")
	tx.Commit()

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
			Uuid: line[9],
		}

		parsedLines = append(parsedLines, newLine)

	}

	return parsedLines
}