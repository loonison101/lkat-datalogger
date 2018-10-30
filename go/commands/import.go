package commands

import (
	"fmt"
	"github.com/jinzhu/gorm"
	"github.com/spf13/cobra"
	"io"
	"io/ioutil"
	"log"
	"../pkg"
	"net/http"
	"os"
	jww "github.com/spf13/jwalterweatherman"
)

type ImportCsvToDbCommand struct {}

func DownloadFile(filepath string, url string) error {

	// Create the file
	out, err := os.Create(filepath)
	if err != nil {
		return err
	}
	defer out.Close()

	// Get the data
	resp, err := http.Get(url)
	if err != nil {
		return err
	}
	defer resp.Body.Close()

	// Write the body to file
	_, err = io.Copy(out, resp.Body)
	if err != nil {
		return err
	}

	return nil
}

func (c ImportCsvToDbCommand) GetCommand(args []string) *cobra.Command {

	var csvFile string

	cmd := &cobra.Command{
		Use: "import",
		Run: func(cmd *cobra.Command, args2 []string) {

			//log.Output(2, fmt.Sprintf("Downloading file: %s", csvFile))
			jww.INFO.Printf("Downloading file: %s", csvFile)
			tempFile, err := ioutil.TempFile("", "downloaded-csv-")
			if err != nil {
				panic(err)
			}
			err = DownloadFile(tempFile.Name(), csvFile)
			if err != nil {
				panic(err)
			}
			defer os.Remove(tempFile.Name())
			jww.INFO.Printf("File downloaded here: %s", tempFile.Name())

			var reader = getCsvReader(tempFile.Name())
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

			log.Output(2, "Beginning populating DB since populateDb is true")
			loadDatabase(csvLines, db)

			log.Output(2, "Done populating DB")
		},
	}

	cmd.Flags().StringVarP(&csvFile, "source", "s", "../LOG.8.25.18.csv", "The source CSV file path to process")

	return cmd
}