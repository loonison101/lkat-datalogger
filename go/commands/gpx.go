package commands

import (
	"../pkg"
	"github.com/jinzhu/gorm"
	_ "github.com/jinzhu/gorm/dialects/sqlite"
	"github.com/spf13/cobra"
	jww "github.com/spf13/jwalterweatherman"
	"github.com/teris-io/shortid"
	"path/filepath"
)

type GpxCommand struct {}

func (c GpxCommand) GetCommand(args []string) *cobra.Command{
	cmd := &cobra.Command{
		Use: "gpx",
		Run: func(cmd *cobra.Command, args2 []string) {

			//log.Output(2, "Connecting to DB...")
			jww.INFO.Printf("Connecting to DB...")

			// Connect to DB
			// TODO: these values need to come from config
			db, err := gorm.Open("sqlite3", "lkat.db")
			if err != nil {
				panic("failed to connect database - " + err.Error())
			}
			defer db.Close()

			// Load ALL our data
			csvLines := []pkg.CsvLine{}

			jww.INFO.Printf( "Loading ALL rows from DB...")
			db.Find(&csvLines)
			jww.INFO.Printf( "Loaded %d rows", len(csvLines))


			jww.INFO.Printf( "Creating GPX file...")

			// Create the gpx object
			gpxObject := createGpx(csvLines)

			defaultDestinationFile, _ := shortid.Generate()
			destinationFile := "../generated/"+defaultDestinationFile+".gpx"

			writeGpxFile(gpxObject, destinationFile)

			absoluteDestinationFile, _ := filepath.Abs(destinationFile)
			jww.INFO.Printf( "Done writing gpx file to %s", absoluteDestinationFile)

		},
	}

	return cmd
}