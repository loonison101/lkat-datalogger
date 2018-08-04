// #include <Arduino.h>
// #include <TinyGPS++.h>
// #include <HardwareSerial.h>

// TinyGPSPlus gps;
// HardwareSerial SerialGPS(1);

// #define TASK_SERIAL_RATE 500
// uint32_t nextSerialTaskTs = 0;

// void setup() {
//     Serial.begin(115200);
//     SerialGPS.begin(9600, SERIAL_8N1, 16, 17);
// }

// void loop() {
//     uint8_t button = digitalRead(PIN_BUTTON);
//     static uint8_t buttonPrevious;

//     // Store new origin point
//     if (button == LOW && buttonPrevious == HIGH) {
//         originLat = gps.location.lat();
//         originLon = gps.location.lng();

//         long writeValue;
//         writeValue = originLat * 1000000;
//         EEPROM_writeAnything(0, writeValue);
//         writeValue = originLon * 1000000;
//         EEPROM_writeAnything(4, writeValue);
//         EEPROM.commit();

//         distMax = 0;
//         altMax = 0;
//         spdMax = 0;
//     }

//     buttonPrevious = button;

//     while (SerialGPS.available() > 0) {
//         gps.encode(SerialGPS.read());
//     }

//     double dist = 0;

//     if (gps.satellites.value() > 4) {
//         dist = TinyGPSPlus::distanceBetween(gps.location.lat(), gps.location.lng(), originLat, originLon);

//         if (dist > distMax && abs(prevDist - dist) < 50) {
//             distMax = dist;
//         }

//         prevDist = dist;

//         if (gps.altitude.meters() > altMax) {
//             altMax = gps.altitude.meters();
//         }

//         if (gps.speed.mps() > spdMax) {
//             spdMax = gps.speed.mps();
//         }
//     }

//     if (nextSerialTaskTs < millis()) {

//         Serial.print("LAT=");  Serial.println(gps.location.lat(), 6);
//         Serial.print("LONG="); Serial.println(gps.location.lng(), 6);
//         Serial.print("ALT=");  Serial.println(gps.altitude.meters());
//         Serial.print("Sats=");  Serial.println(gps.satellites.value());
//         Serial.print("DST: ");
//         Serial.println(dist,1);

//         nextSerialTaskTs = millis() + TASK_SERIAL_RATE;
//     }
// }

// Test code for Adafruit GPS modules using MTK3329/MTK3339 driver
//
// This code shows how to listen to the GPS module in an interrupt
// which allows the program to have more 'freedom' - just parse
// when a new NMEA sentence is available! Then access data when
// desired.
//
// Tested and works great with the Adafruit Ultimate GPS module
// using MTK33x9 chipset
//    ------> http://www.adafruit.com/products/746
// Pick one up today at the Adafruit electronics shop 
// and help support open source hardware & software! -ada

// #include <Adafruit_GPS.h>
// // #include <SoftwareSerial.h>

// // If you're using a GPS module:
// // Connect the GPS Power pin to 5V
// // Connect the GPS Ground pin to ground
// // If using software serial (sketch example default):
// //   Connect the GPS TX (transmit) pin to Digital 3
// //   Connect the GPS RX (receive) pin to Digital 2
// // If using hardware serial (e.g. Arduino Mega):
// //   Connect the GPS TX (transmit) pin to Arduino RX1, RX2 or RX3
// //   Connect the GPS RX (receive) pin to matching TX1, TX2 or TX3

// // If you're using the Adafruit GPS shield, change 
// // SoftwareSerial mySerial(3, 2); -> SoftwareSerial mySerial(8, 7);
// // and make sure the switch is set to SoftSerial

// // If using software serial, keep this line enabled
// // (you can change the pin numbers to match your wiring):
// // SoftwareSerial mySerial(16, 17);

// // If using hardware serial (e.g. Arduino Mega), comment out the
// // above SoftwareSerial line, and enable this line instead
// // (you can change the Serial number to match your wiring):

// HardwareSerial mySerial = Serial1;
// // HardwareSerial mySerial(16,17);


// Adafruit_GPS GPS(&mySerial);


// // Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// // Set to 'true' if you want to debug and listen to the raw GPS sentences. 
// #define GPSECHO  true

// // this keeps track of whether we're using the interrupt
// // off by default!
// boolean usingInterrupt = false;
// // void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy

// void setup()  
// {
    
//   // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
//   // also spit it out
//   Serial.begin(115200);
//   Serial.println("Adafruit GPS library basic test!");

//   // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
//   GPS.begin(9600);
  
//   // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
//   GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
//   // uncomment this line to turn on only the "minimum recommended" data
//   //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
//   // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
//   // the parser doesn't care about other sentences at this time
  
//   // Set the update rate
//   GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
//   // For the parsing code to work nicely and have time to sort thru the data, and
//   // print it out we don't suggest using anything higher than 1 Hz

//   // Request updates on antenna status, comment out to keep quiet
//   GPS.sendCommand(PGCMD_ANTENNA);

//   // the nice thing about this code is you can have a timer0 interrupt go off
//   // every 1 millisecond, and read data from the GPS for you. that makes the
//   // loop code a heck of a lot easier!
// //   useInterrupt(true);

//   delay(1000);
//   // Ask for firmware version
//   mySerial.println(PMTK_Q_RELEASE);
// }


// // Interrupt is called once a millisecond, looks for any new GPS data, and stores it
// // SIGNAL(TIMER0_COMPA_vect) {
// //   char c = GPS.read();
// //   // if you want to debug, this is a good time to do it!
// // #ifdef UDR0
// //   if (GPSECHO)
// //     if (c) UDR0 = c;  
// //     // writing direct to UDR0 is much much faster than Serial.print 
// //     // but only one character can be written at a time. 
// // #endif
// // }

// // void useInterrupt(boolean v) {
// //   if (v) {
// //     // Timer0 is already used for millis() - we'll just interrupt somewhere
// //     // in the middle and call the "Compare A" function above
// //     OCR0A = 0xAF;
// //     TIMSK0 |= _BV(OCIE0A);
// //     usingInterrupt = true;
// //   } else {
// //     // do not call the interrupt function COMPA anymore
// //     TIMSK0 &= ~_BV(OCIE0A);
// //     usingInterrupt = false;
// //   }
// // }

// uint32_t timer = millis();
// void loop()                     // run over and over again
// {
//   // in case you are not using the interrupt above, you'll
//   // need to 'hand query' the GPS, not suggested :(
//   if (! usingInterrupt) {
//     // read data from the GPS in the 'main loop'
//     char c = GPS.read();
//     // if you want to debug, this is a good time to do it!
//     if (GPSECHO)
//       if (c) Serial.print(c);
//   }
  
//   // if a sentence is received, we can check the checksum, parse it...
//   if (GPS.newNMEAreceived()) {
//     // a tricky thing here is if we print the NMEA sentence, or data
//     // we end up not listening and catching other sentences! 
//     // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
//     //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false
  
//     if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
//       return;  // we can fail to parse a sentence in which case we should just wait for another
//   }

//   // if millis() or timer wraps around, we'll just reset it
//   if (timer > millis())  timer = millis();

//   // approximately every 2 seconds or so, print out the current stats
//   if (millis() - timer > 2000) { 
//     timer = millis(); // reset the timer
    
//     Serial.print("\nTime: ");
//     Serial.print(GPS.hour, DEC); Serial.print(':');
//     Serial.print(GPS.minute, DEC); Serial.print(':');
//     Serial.print(GPS.seconds, DEC); Serial.print('.');
//     Serial.println(GPS.milliseconds);
//     Serial.print("Date: ");
//     Serial.print(GPS.day, DEC); Serial.print('/');
//     Serial.print(GPS.month, DEC); Serial.print("/20");
//     Serial.println(GPS.year, DEC);
//     Serial.print("Fix: "); Serial.print((int)GPS.fix);
//     Serial.print(" quality: "); Serial.println((int)GPS.fixquality); 
//     if (GPS.fix) {
//       Serial.print("Location: ");
//       Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
//       Serial.print(", "); 
//       Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
//       Serial.print("Location (in degrees, works with Google Maps): ");
//       Serial.print(GPS.latitudeDegrees, 4);
//       Serial.print(", "); 
//       Serial.println(GPS.longitudeDegrees, 4);
      
//       Serial.print("Speed (knots): "); Serial.println(GPS.speed);
//       Serial.print("Angle: "); Serial.println(GPS.angle);
//       Serial.print("Altitude: "); Serial.println(GPS.altitude);
//       Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
//     }
//   }
// }


// Test code for Ultimate GPS Using Hardware Serial (e.g. GPS Flora or FeatherWing)
//
// This code shows how to listen to the GPS module via polling. Best used with
// Feathers or Flora where you have hardware Serial and no interrupt
//
// Tested and works great with the Adafruit GPS FeatherWing
// ------> https://www.adafruit.com/products/3133
// or Flora GPS
// ------> https://www.adafruit.com/products/1059
// but also works with the shield, breakout
// ------> https://www.adafruit.com/products/1272
// ------> https://www.adafruit.com/products/746
// 
// Pick one up today at the Adafruit electronics shop
// and help support open source hardware & software! -ada
     
// #include <Adafruit_GPS.h>

// // what's the name of the hardware serial port?
// #define GPSSerial Serial1

// // Connect to the GPS on the hardware port
// Adafruit_GPS GPS(&GPSSerial);
     
// // Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// // Set to 'true' if you want to debug and listen to the raw GPS sentences
// #define GPSECHO true

// uint32_t timer = millis();


// void setup()
// {
//   //while (!Serial);  // uncomment to have the sketch wait until Serial is ready
  
//   // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
//   // also spit it out
//   Serial.begin(115200);
//   Serial.println("Adafruit GPS library basic test!");
     
//   // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
//   GPS.begin(9600);
//   // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
//   GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
//   // uncomment this line to turn on only the "minimum recommended" data
//   //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
//   // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
//   // the parser doesn't care about other sentences at this time
//   // Set the update rate
//   GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
//   // For the parsing code to work nicely and have time to sort thru the data, and
//   // print it out we don't suggest using anything higher than 1 Hz
     
//   // Request updates on antenna status, comment out to keep quiet
//   GPS.sendCommand(PGCMD_ANTENNA);

//   delay(1000);
  
//   // Ask for firmware version
//   GPSSerial.println(PMTK_Q_RELEASE);
// }

// void loop() // run over and over again
// {
//   // read data from the GPS in the 'main loop'
//   char c = GPS.read();
//   // if you want to debug, this is a good time to do it!
//   if (GPSECHO)
//     if (c) Serial.print(c);
//   // if a sentence is received, we can check the checksum, parse it...
//   if (GPS.newNMEAreceived()) {
//     // a tricky thing here is if we print the NMEA sentence, or data
//     // we end up not listening and catching other sentences!
//     // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
//     Serial.println(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
//     if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
//       return; // we can fail to parse a sentence in which case we should just wait for another
//   }
//   // if millis() or timer wraps around, we'll just reset it
//   if (timer > millis()) timer = millis();
     
//   // approximately every 2 seconds or so, print out the current stats
//   if (millis() - timer > 2000) {
//     timer = millis(); // reset the timer
//     Serial.print("\nTime: ");
//     Serial.print(GPS.hour, DEC); Serial.print(':');
//     Serial.print(GPS.minute, DEC); Serial.print(':');
//     Serial.print(GPS.seconds, DEC); Serial.print('.');
//     Serial.println(GPS.milliseconds);
//     Serial.print("Date: ");
//     Serial.print(GPS.day, DEC); Serial.print('/');
//     Serial.print(GPS.month, DEC); Serial.print("/20");
//     Serial.println(GPS.year, DEC);
//     Serial.print("Fix: "); Serial.print((int)GPS.fix);
//     Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
//     if (GPS.fix) {
//       Serial.print("Location: ");
//       Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
//       Serial.print(", ");
//       Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
//       Serial.print("Speed (knots): "); Serial.println(GPS.speed);
//       Serial.print("Angle: "); Serial.println(GPS.angle);
//       Serial.print("Altitude: "); Serial.println(GPS.altitude);
//       Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
//     }
//   }
// }



// Test code for Adafruit GPS modules using MTK3329/MTK3339 driver
//
// This code just echos whatever is coming from the GPS unit to the
// serial monitor, handy for debugging!
//
// Tested and works great with the Adafruit Ultimate GPS module
// using MTK33x9 chipset
//    ------> http://www.adafruit.com/products/746
// Pick one up today at the Adafruit electronics shop 
// and help support open source hardware & software! -ada

// #include <Adafruit_GPS.h>
// #if ARDUINO >= 100
//  #include <SoftwareSerial.h>
// #else
//   // Older Arduino IDE requires NewSoftSerial, download from:
//   // http://arduiniana.org/libraries/newsoftserial/
// // #include <NewSoftSerial.h>
//  // DO NOT install NewSoftSerial if using Arduino 1.0 or later!
// #endif

// // Connect the GPS Power pin to 5V
// // Connect the GPS Ground pin to ground
// // If using software serial (sketch example default):
// //   Connect the GPS TX (transmit) pin to Digital 3
// //   Connect the GPS RX (receive) pin to Digital 2
// // If using hardware serial (e.g. Arduino Mega):
// //   Connect the GPS TX (transmit) pin to Arduino RX1, RX2 or RX3
// //   Connect the GPS RX (receive) pin to matching TX1, TX2 or TX3

// // If using software serial, keep these lines enabled
// // (you can change the pin numbers to match your wiring):
// #if ARDUINO >= 100
//   SoftwareSerial mySerial(33, 27, false, 256);
// #else
//   NewSoftSerial mySerial(3, 2);
// #endif
// Adafruit_GPS GPS(&mySerial);
// // If using hardware serial (e.g. Arduino Mega), comment
// // out the above six lines and enable this line instead:
// //Adafruit_GPS GPS(&Serial1);


// // Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// // Set to 'true' if you want to debug and listen to the raw GPS sentences
// #define GPSECHO  true

// // this keeps track of whether we're using the interrupt
// // off by default!
// boolean usingInterrupt = false;
// void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy

// void setup()  
// {    
//   // connect at 115200 so we can read the GPS fast enuf and
//   // also spit it out
//   Serial.begin(115200);
//   Serial.println("Adafruit GPS library basic test!");

//   // 9600 NMEA is the default baud rate for MTK - some use 4800
//   GPS.begin(9600);
  
//   // You can adjust which sentences to have the module emit, below
  
//   // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
//   GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
//   // uncomment this line to turn on only the "minimum recommended" data for high update rates!
//   //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
//   // uncomment this line to turn on all the available data - for 9600 baud you'll want 1 Hz rate
//   //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_ALLDATA);
  
//   // Set the update rate
//   // Note you must send both commands below to change both the output rate (how often the position
//   // is written to the serial line), and the position fix rate.
//   // 1 Hz update rate
//   //GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
//   //GPS.sendCommand(PMTK_API_SET_FIX_CTL_1HZ);
//   // 5 Hz update rate- for 9600 baud you'll have to set the output to RMC or RMCGGA only (see above)
//   GPS.sendCommand(PMTK_SET_NMEA_UPDATE_5HZ);
//   GPS.sendCommand(PMTK_API_SET_FIX_CTL_5HZ);
//   // 10 Hz update rate - for 9600 baud you'll have to set the output to RMC only (see above)
//   // Note the position can only be updated at most 5 times a second so it will lag behind serial output.
//   //GPS.sendCommand(PMTK_SET_NMEA_UPDATE_10HZ);
//   //GPS.sendCommand(PMTK_API_SET_FIX_CTL_5HZ);

//   // Request updates on antenna status, comment out to keep quiet
//   GPS.sendCommand(PGCMD_ANTENNA);

//   // the nice thing about this code is you can have a timer0 interrupt go off
//   // every 1 millisecond, and read data from the GPS for you. that makes the
//   // loop code a heck of a lot easier!
//   useInterrupt(true);
  
//   delay(1000);
// }

// // Interrupt is called once a millisecond, looks for any new GPS data, and stores it
// SIGNAL(TIMER0_COMPA_vect) {
//   char c = GPS.read();
//   // if you want to debug, this is a good time to do it!
//   if (GPSECHO)
//     if (c) UDR0 = c;  
//     // writing direct to UDR0 is much much faster than Serial.print 
//     // but only one character can be written at a time. 
// }

// void useInterrupt(boolean v) {
//   if (v) {
//     // Timer0 is already used for millis() - we'll just interrupt somewhere
//     // in the middle and call the "Compare A" function above
//     OCR0A = 0xAF;
//     TIMSK0 |= _BV(OCIE0A);
//     usingInterrupt = true;
//   } else {
//     // do not call the interrupt function COMPA anymore
//     TIMSK0 &= ~_BV(OCIE0A);
//     usingInterrupt = false;
//   }
// }


// void loop()                     // run over and over again
// {
//    // do nothing! all reading and printing is done in the interrupt
// }

// #include <Arduino.h>
// #include <SoftwareSerial.h>

// // SoftwareSerial swSer(33, 27, false, 115200);
// SoftwareSerial swSer(27, 33, false, 115200);

// //SoftwareSerial swSer(14, 12, false, 256);

// void setup() {
//   Serial.begin(115200);
//   swSer.begin(9600);

//   Serial.println("\nSoftware serial test started");

//   for (char ch = ' '; ch <= 'z'; ch++) {
//     swSer.write(ch);
//   }
//   swSer.println("");

// }

// void loop() {
//   while (swSer.available() > 0) {
//     Serial.write(swSer.read());
//   }
//   while (Serial.available() > 0) {
//     swSer.write(Serial.read());
//   }

// }

#include <Arduino.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <Adafruit_GPS.h>

TinyGPSPlus gps;
HardwareSerial SerialGPS(1);

#define TASK_SERIAL_RATE 1000

uint32_t nextSerialTaskTs = 0;

//Adafruit_GPS GPS(SerialGPS);

void setup() {
    Serial.begin(115200);
    SerialGPS.begin(9600, SERIAL_8N1, 16, 17);
}

void  loop() {
    while (SerialGPS.available() > 0) {
        gps.encode(SerialGPS.read());
    }

    if (nextSerialTaskTs < millis()) {

        Serial.print("satellites=");  Serial.println(gps.satellites.value(), 6);
        Serial.print("LAT=");  Serial.println(gps.location.lat(), 6);
        Serial.print("LONG="); Serial.println(gps.location.lng(), 6);
        Serial.print("ALT=");  Serial.println(gps.altitude.meters());
        Serial.print("Sats=");  Serial.println(gps.satellites.value());
        // Serial.print("DST: ");

        nextSerialTaskTs = millis() + TASK_SERIAL_RATE;
    }

    
}