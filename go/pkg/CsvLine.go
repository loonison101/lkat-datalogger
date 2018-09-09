package pkg

import (
	"github.com/jinzhu/gorm"
	"time"
)

type CsvLine struct {
	gorm.Model
	Satellites int
	Hdop       float64
	Latitude   float64
	Longitude  float64
	Age        float64
	When       time.Time
	Altitude   float64
	Uuid       string
	Speed   float64
	RawLine string
}
