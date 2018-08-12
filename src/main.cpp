#include <HardwareSerial.h>
#include <TinyGPS++.h>
#include <SPI.h>
#include <mySD.h>
#include <WiFi.h>
// #include <Wire.h>
// #include <Adafruit_MPL3115A2.h>

Sd2Card card;
SdVolume volume;
SdFile root;
File logFile;
// Adafruit_MPL3115A2 baro = Adafruit_MPL3115A2();
//TwoWire *wire;
// TwoWire stupid = TwoWire(1);
// wire = &stupid;


const int chipSelect = 33;  

HardwareSerial MySerial(1);
TinyGPSPlus gps;
uint32_t nextSerialTaskTs = 0;
#define TASK_SERIAL_RATE 1000 * 1

// char dataRow[1000];

void setup() {
    Serial.begin(115200);
    MySerial.begin(9600, SERIAL_8N1, 16, 17);
    // Wire.begin(22,23);
    // stupid.begin(22,23);

    // while(!Serial){}

    WiFi.mode(WIFI_OFF);
    btStop();

    pinMode(chipSelect, OUTPUT);

    Serial.println("trying");
    // Serial.println("sda: " + SDA);
    // Serial.println("SCL: " + SCL);

    while (!SD.begin(33, 18, 19, 5)) {
        Serial.println("initialization failed. Things to check:");
        Serial.println("* is a card is inserted?");
        Serial.println("* Is your wiring correct?");
        Serial.println("* did you change the chipSelect pin to match your shield or module?");
    }

    // if (! baro.begin(&stupid)) {
    //   Serial.println("Couldnt find baro sensor");
    // }

//     // print the type of card
//     Serial.print("\nCard type: ");
//     switch(card.type()) {
//         case SD_CARD_TYPE_SD1:
//         Serial.println("SD1");
//         break;
//         case SD_CARD_TYPE_SD2:
//         Serial.println("SD2");
//         break;
//         case SD_CARD_TYPE_SDHC:
//         Serial.println("SDHC");
//         break;
//         default:
//         Serial.println("Unknown");
//     }

//     // print the type and size of the first FAT-type volume
//   uint32_t volumesize;
//   Serial.print("\nVolume type is FAT");
//   Serial.println(volume.fatType(), DEC);
//   Serial.println();
  
//   volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
//   volumesize *= volume.clusterCount();       // we'll have a lot of clusters
//   volumesize *= 512;                            // SD card blocks are always 512 bytes
//   Serial.print("Volume size (bytes): ");
//   Serial.println(volumesize);
//   Serial.print("Volume size (Kbytes): ");
//   volumesize /= 1024;
//   Serial.println(volumesize);
//   Serial.print("Volume size (Mbytes): ");
//   volumesize /= 1024;
//   Serial.println(volumesize);

  
//   Serial.println("\nFiles found on the card (name, date and size in bytes): ");
//   root.openRoot(volume);
  
//   // list all files in the card with date and size
//   root.ls(LS_R | LS_DATE | LS_SIZE);

    Serial.println("i hate everything");
}

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (MySerial.available())
      gps.encode(MySerial.read());
  } while (millis() - start < ms);
}


static void printFloat(float val, bool valid, int len, int prec)
{
  if (!valid)
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else
  {
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
      Serial.print(' ');
  }
  smartDelay(0);
}

static void printInt(unsigned long val, bool valid, int len)
{
  char sz[32] = "*****************";
  if (valid)
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i=strlen(sz); i<len; ++i)
    sz[i] = ' ';
  if (len > 0) 
    sz[len-1] = ' ';
  Serial.print(sz);
  smartDelay(0);
}

static void printDateTime(TinyGPSDate &d, TinyGPSTime &t)
{
  if (!d.isValid())
  {
    Serial.print(F("********** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
    Serial.print(sz);
  }
  
  if (!t.isValid())
  {
    Serial.print(F("******** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
    Serial.print(sz);
  }

  printInt(d.age(), d.isValid(), 5);
  smartDelay(0);
}


static void printStr(const char *str, int len)
{
  int slen = strlen(str);
  for (int i=0; i<len; ++i)
    Serial.print(i<slen ? str[i] : ' ');
  smartDelay(0);
}

void displayInfo()
{
    // Serial.println("custom satellites: " + gps.satellites.value());
    printInt(gps.satellites.value(), gps.satellites.isValid(), 5);
  printFloat(gps.hdop.hdop(), gps.hdop.isValid(), 6, 1);
  printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
  printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);
  printInt(gps.location.age(), gps.location.isValid(), 5);
  printDateTime(gps.date, gps.time);
  printFloat(gps.altitude.meters(), gps.altitude.isValid(), 7, 2);
  printFloat(gps.course.deg(), gps.course.isValid(), 7, 2);
  printFloat(gps.speed.mph(), gps.speed.isValid(), 6, 2);

  printStr(gps.course.isValid() ? TinyGPSPlus::cardinal(gps.course.deg()) : "*** ", 6);

  Serial.println();

  if (!gps.location.isValid()) {
    Serial.println("Not persisting gps, lat/long is not valid");
    return;
  }

  char buffer[1000];
  sprintf(buffer, "%ld,%0.2f,%f, %f,%ld,%02d/%02d/%02d,%02d:%02d:%02d,%f,%f", 
        gps.satellites.value(),
        gps.hdop.hdop(),
        gps.location.lat(),
        gps.location.lng(),
        gps.location.age(),
        
        gps.date.month(),
        gps.date.day(),
        gps.date.year(),

        gps.time.hour(),
        gps.time.minute(),
        gps.time.second(),

        gps.altitude.meters(),
        gps.speed.mph()



        );

    // Serial.println("custom -- " + String(buffer));
        Serial.println("trying to write to sd card");
        logFile = SD.open("log.txt", FILE_WRITE);

        if (logFile) {
            Serial.println("writing to log.txt");
            logFile.println(String(buffer));
            logFile.close();
            Serial.println("done writing");
        } else {
            Serial.println("error opening log.txt");
        }

}
void loop() {
    while (MySerial.available() > 0) {
        gps.encode(MySerial.read());
    }

    // while (MySerial.available() > 0) {
    //     uint8_t byteFromSerial = MySerial.read();
    //     Serial.write(byteFromSerial);
    // }

    if (nextSerialTaskTs < millis()) {
            
            // Do something
            //if (gps.encode(byteFromSerial)) {
                displayInfo();

                // float pascals = baro.getPressure();
                // // Our weather page presents pressure in Inches (Hg)
                // // Use http://www.onlineconversion.com/pressure.htm for other units
                // Serial.print(pascals/3377); Serial.println(" Inches (Hg)");

                // float altm = baro.getAltitude();
                // Serial.print(altm); Serial.println(" meters");

                // float tempC = baro.getTemperature();
                // Serial.print(tempC); Serial.println("*C");
                
                
                
            // } else {
            //     Serial.println("got false?");
            // }

            nextSerialTaskTs = millis() + TASK_SERIAL_RATE;
        }

    
}


// --------------------------------------
// i2c_scanner

// Version 1
//    This program (or code that looks like it)
//    can be found in many places.
//    For example on the Arduino.cc forum.
//    The original author is not know.
// Version 2, Juni 2012, Using Arduino 1.0.1
//     Adapted to be as simple as possible by Arduino.cc user Krodal
// Version 3, Feb 26  2013
//    V3 by louarnold
// Version 4, March 3, 2013, Using Arduino 1.0.3
//    by Arduino.cc user Krodal.
//    Changes by louarnold removed.
//    Scanning addresses changed from 0...127 to 1...119,
//    according to the i2c scanner by Nick Gammon
//    http://www.gammon.com.au/forum/?id=10896
// Version 5, March 28, 2013
//    As version 4, but address scans now to 127.
//    A sensor seems to use address 120.
// Version 6, November 27, 2015.
//    Added waiting for the Leonardo serial communication.


// This sketch tests the standard 7-bit addresses
// Devices with higher bit address might not be seen properly.

 
// #include <Wire.h>
// #include <Arduino.h>
 
 
// void setup()
// {
//   Wire.begin();
 
//   Serial.begin(115200);
//   while (!Serial);             // Leonardo: wait for serial monitor
//   Serial.println("\nI2C Scanner");
// }
 
 
// void loop()
// {
//   byte error, address;
//   int nDevices;
 
//   Serial.println("Scanning...");
 
//   nDevices = 0;
//   for(address = 1; address < 127; address++ )
//   {
//     // The i2c_scanner uses the return value of
//     // the Write.endTransmisstion to see if
//     // a device did acknowledge to the address.
//     Wire.beginTransmission(address);
//     error = Wire.endTransmission();
 
//     if (error == 0)
//     {
//       Serial.print("I2C device found at address 0x");
//       if (address<16)
//         Serial.print("0");
//       Serial.print(address,HEX);
//       Serial.println("  !");
 
//       nDevices++;
//     }
//     else if (error==4)
//     {
//       Serial.print("Unknown error at address 0x");
//       if (address<16)
//         Serial.print("0");
//       Serial.println(address,HEX);
//     }    
//   }
//   if (nDevices == 0)
//     Serial.println("No I2C devices found\n");
//   else
//     Serial.println("done\n");
 
//   delay(5000);           // wait 5 seconds for next scan
// }

// #include <Arduino.h>
// #include <Wire.h>
// #include <Adafruit_MPL3115A2.h>

// // Power by connecting Vin to 3-5V, GND to GND
// // Uses I2C - connect SCL to the SCL pin, SDA to SDA pin
// // See the Wire tutorial for pinouts for each Arduino
// // http://arduino.cc/en/reference/wire
// Adafruit_MPL3115A2 baro = Adafruit_MPL3115A2();
// // TwoWire *wire;
// TwoWire wire = TwoWire(1);
// // wire = &stupid;


// void setup() {
//   Serial.begin(115200);
//   wire.begin();
//   // Wire.begin(23,22);
//   // Wire1.begin(23,22);
//   Serial.println("Adafruit_MPL3115A2 test!");
// }

// void loop() {
//   if (! baro.begin(&wire)) {
//     Serial.println("Couldnt find sensor");
//     return;
//   }
  
//   float pascals = baro.getPressure();
//   // Our weather page presents pressure in Inches (Hg)
//   // Use http://www.onlineconversion.com/pressure.htm for other units
//   Serial.print(pascals/3377); Serial.println(" Inches (Hg)");

//   float altm = baro.getAltitude();
//   Serial.print(altm); Serial.println(" meters");

//   float tempC = baro.getTemperature();
//   Serial.print(tempC); Serial.println("*C");

//   delay(250);
// }

// #include <Wire.h>
// #include "SparkFunMPL3115A2.h"

// //Create an instance of the object
// MPL3115A2 myPressure;

// void setup()
// {
//   Wire.begin();        // Join i2c bus
//   Serial.begin(115200);  // Start serial for output

//   myPressure.begin(); // Get sensor online

//   //Configure the sensor
//   myPressure.setModeAltimeter(); // Measure altitude above sea level in meters
//   //myPressure.setModeBarometer(); // Measure pressure in Pascals from 20 to 110 kPa

//   myPressure.setOversampleRate(7); // Set Oversample to the recommended 128
//   myPressure.enableEventFlags(); // Enable all three pressure and temp event flags 
// }

// void loop()
// {
//   //float altitude = myPressure.readAltitude();
//   //Serial.print("Altitude(m):");
//   //Serial.print(altitude, 2);

//   float altitude = myPressure.readAltitudeFt();
//   Serial.print(" Altitude(ft):");
//   Serial.print(altitude, 2);

//   //float pressure = myPressure.readPressure();
//   //Serial.print("Pressure(Pa):");
//   //Serial.print(pressure, 2);

//   //float temperature = myPressure.readTemp();
//   //Serial.print(" Temp(c):");
//   //Serial.print(temperature, 2);

//   float temperature = myPressure.readTempF();
//   Serial.print(" Temp(f):");
//   Serial.print(temperature, 2);

//   Serial.println();
// }

// #include <Wire.h>
// #include <Adafruit_MPL3115A2.h>

// // Power by connecting Vin to 3-5V, GND to GND
// // Uses I2C - connect SCL to the SCL pin, SDA to SDA pin
// // See the Wire tutorial for pinouts for each Arduino
// // http://arduino.cc/en/reference/wire
// Adafruit_MPL3115A2 baro = Adafruit_MPL3115A2();

// void setup() {
//   Serial.begin(115200);
//   Serial.println("Adafruit_MPL3115A2 test!");
// }

// void loop() {
//   if (! baro.begin()) {
//     Serial.println("Couldnt find sensor");
//     return;
//   }
  
//   float pascals = baro.getPressure();
//   // Our weather page presents pressure in Inches (Hg)
//   // Use http://www.onlineconversion.com/pressure.htm for other units
//   Serial.print(pascals/3377); Serial.println(" Inches (Hg)");

//   float altm = baro.getAltitude();
//   Serial.print(altm); Serial.println(" meters");

//   float tempC = baro.getTemperature();
//   Serial.print(tempC); Serial.println("*C");

//   delay(250);
// }