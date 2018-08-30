package pkg

import "time"

type CsvLine struct {
	Satellites int
	Hdop       float64
	Latitude   float64
	Longitude  float64
	Age        float64
	When       time.Time
	Altitude   float64

	Speed   float64
	RawLine string
}
