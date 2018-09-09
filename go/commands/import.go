package commands

import (
	"fmt"
	"github.com/jinzhu/gorm"
	"github.com/spf13/cobra"
	"log"
	"../pkg"
)

type ImportCsvToDbCommand struct {}

func (c ImportCsvToDbCommand) GetCommand(args []string) *cobra.Command {

	var csvFile string

	cmd := &cobra.Command{
		Use: "import",
		Run: func(cmd *cobra.Command, args2 []string) {


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

			log.Output(2, "Beginning populating DB since populateDb is true")
			loadDatabase(csvLines, db)

			log.Output(2, "Done populating DB")
		},
	}

	cmd.Flags().StringVarP(&csvFile, "source", "s", "../LOG.8.25.18.csv", "The source CSV file path to process")

	return cmd
}