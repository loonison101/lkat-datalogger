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

// #include <Arduino.h>
// #include <TinyGPS++.h>
// #include <HardwareSerial.h>
// // #include <Adafruit_GPS.h>

// TinyGPSPlus gps;
// HardwareSerial SerialGPS(1);

// #define TASK_SERIAL_RATE 1000

// uint32_t nextSerialTaskTs = 0;

// //Adafruit_GPS GPS(SerialGPS);

// void setup() {
//     Serial.begin(115200);
//     SerialGPS.begin(9600, SERIAL_8N1, 16, 17);
// }

// void  loop() {
//     while (SerialGPS.available() > 0) {
//         gps.encode(SerialGPS.read());
//     }

//     // Serial.print("satellites"); 
//     // Serial.print(gps.satellites.value(), 6);

//     if (nextSerialTaskTs < millis()) {

        
//         Serial.print("satellites=");  Serial.println(gps.satellites.value(), 6);
//         Serial.print("LAT=");  Serial.println(gps.location.lat(), 6);
//         Serial.print("LONG="); Serial.println(gps.location.lng(), 6);
//         Serial.print("ALT=");  Serial.println(gps.altitude.meters());
//         Serial.print("Sats=");  Serial.println(gps.satellites.value());
//         // Serial.print("DST: ");

//         nextSerialTaskTs = millis() + TASK_SERIAL_RATE;
//     }

    
// }

// #include <TinyGPS++.h>
// // #include <SoftwareSerial.h>
// #include <HardwareSerial.h>

// static const int RXPin = 16, TXPin = 17;
// static const uint32_t GPSBaud = 9600;

// TinyGPSPlus gps;
// // SoftwareSerial ss(RXPin, TXPin);
// HardwareSerial SerialGPS(1);

// void displayInfo()
// {
//   Serial.print(F("Location: ")); 
//   if (gps.location.isValid())
//   {
//     Serial.print(gps.location.lat(), 6);
//     Serial.print(F(","));
//     Serial.print(gps.location.lng(), 6);
//   }
//   else
//   {
//     Serial.print(F("INVALID"));
//   }

//   Serial.print(F("  Date/Time: "));
//   if (gps.date.isValid())
//   {
//     Serial.print(gps.date.month());
//     Serial.print(F("/"));
//     Serial.print(gps.date.day());
//     Serial.print(F("/"));
//     Serial.print(gps.date.year());
//   }
//   else
//   {
//     Serial.print(F("INVALID"));
//   }

//   Serial.print(F(" "));
//   if (gps.time.isValid())
//   {
//     if (gps.time.hour() < 10) Serial.print(F("0"));
//     Serial.print(gps.time.hour());
//     Serial.print(F(":"));
//     if (gps.time.minute() < 10) Serial.print(F("0"));
//     Serial.print(gps.time.minute());
//     Serial.print(F(":"));
//     if (gps.time.second() < 10) Serial.print(F("0"));
//     Serial.print(gps.time.second());
//     Serial.print(F("."));
//     if (gps.time.centisecond() < 10) Serial.print(F("0"));
//     Serial.print(gps.time.centisecond());
//   }
//   else
//   {
//     Serial.print(F("INVALID"));
//   }

//   Serial.println();
// }

// void setup()
// {
//   Serial.begin(115200);
//   ss.begin(GPSBaud);

//   Serial.println(F("DeviceExample.ino"));
//   Serial.println(F("A simple demonstration of TinyGPS++ with an attached GPS module"));
//   Serial.print(F("Testing TinyGPS++ library v. ")); Serial.println(TinyGPSPlus::libraryVersion());
//   Serial.println(F("by Mikal Hart"));
//   Serial.println();
// }

// void loop()
// {
//   // This sketch displays information every time a new sentence is correctly encoded.
//   while (ss.available() > 0)
//     if (gps.encode(ss.read()))
//       displayInfo();

//   if (millis() > 5000 && gps.charsProcessed() < 10)
//   {
//     Serial.println(F("No GPS detected: check wiring."));
//     while(true);
//   }
// }

// #include <Arduino.h>
// #include <TinyGPS++.h>
// #include <HardwareSerial.h>
// #include <Adafruit_GPS.h>

// TinyGPSPlus gps;
// HardwareSerial SerialGPS(1);

// #define TASK_SERIAL_RATE 2000

// uint32_t nextSerialTaskTs = 0;

// //Adafruit_GPS GPS(SerialGPS);

// void setup() {
//     // put your setup code here, to run once:
//     Serial.begin(115200);
//     SerialGPS.begin(9600, SERIAL_8N1, 16, 17);
// }

// void  loop() {
//     while (SerialGPS.available() > 0) {
//         Serial.print(char(SerialGPS.read()));
//         gps.encode(SerialGPS.read());
//     }

//     if (nextSerialTaskTs < millis()) {
         


//         Serial.println("satellites=");  Serial.println(gps.satellites.value(), 6);
//         Serial.println("LAT=");  Serial.println(gps.location.lat(), 6);
//         // Serial.print("LONG="); Serial.println(gps.location.lng(), 6);
//         // Serial.print("ALT=");  Serial.println(gps.altitude.meters());
//         // Serial.print("Sats=");  Serial.println(gps.satellites.value());
//         // // Serial.print("DST: ");

//         nextSerialTaskTs = millis() + TASK_SERIAL_RATE;
//     }

    
// }


// #include <Arduino.h>
// // #include <TinyGPS++.h>
// // #include <HardwareSerial.h>
// #include <SoftwareSerial.h>
// #include <Adafruit_GPS.h>

// // TinyGPSPlus gps;
// //HardwareSerial SerialGPS(1);
// SoftwareSerial swSer(16, 17, false, 256);

// void setup() {
//   Serial.begin(9600);
//   swSer.begin(9600);

//   Serial.println("\nSoftware serial test started");

//   for (char ch = ' '; ch <= 'z'; ch++) {
//     swSer.write(ch);
//   }
//   swSer.println("aaaaaaaaa");

// }

// void loop() {
//   while (swSer.available() > 0) {
//     Serial.write(swSer.read());
//   }
//   while (Serial.available() > 0) {
//     swSer.write(Serial.read());
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
#include <HardwareSerial.h>
#include <TinyGPS++.h>
#include <SPI.h>
#include <mySD.h>

Sd2Card card;
SdVolume volume;
SdFile root;
File logFile;


const int chipSelect = 33;  

HardwareSerial MySerial(1);
TinyGPSPlus gps;
uint32_t nextSerialTaskTs = 0;
#define TASK_SERIAL_RATE 5000

// char dataRow[1000];

void setup() {
    Serial.begin(9600);
    MySerial.begin(9600, SERIAL_8N1, 16, 17);

    pinMode(chipSelect, OUTPUT);

    Serial.println("trying");

    while (!SD.begin(33, 18, 19, 5)) {
        Serial.println("initialization failed. Things to check:");
        Serial.println("* is a card is inserted?");
        Serial.println("* Is your wiring correct?");
        Serial.println("* did you change the chipSelect pin to match your shield or module?");
    }

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
                
                
                
            // } else {
            //     Serial.println("got false?");
            // }

            nextSerialTaskTs = millis() + TASK_SERIAL_RATE;
        }

    
}