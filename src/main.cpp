#include <HardwareSerial.h>
#include <TinyGPS++.h>
#include <SPI.h>
#include <mySD.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "secrets.h"
#include <test.h>
#include <Chrono.h>
#include <StringHelper.h>
#include <FileHelper.h>

Sd2Card card;
SdVolume volume;
SdFile root;
File logFile;

const int chipSelect = 33;

HardwareSerial MySerial(1);
TinyGPSPlus gps;
uint32_t nextSerialTaskTs = 0;

double lastLatitude = 0;
double lastLongitude = 0;

// For 2 seconds show the LED so we know we are up and running
uint32_t statusLedLife;
HTTPClient http;

String output;
void setup()
{
  Serial.begin(115200);
  delay(100);
  Serial.println("Starting up...");
  // MySerial.begin(9600, SERIAL_8N1, 16, 17);

  // WiFi.mode(WIFI_OFF);
  // btStop();

  // pinMode(chipSelect, OUTPUT);
  // pinMode(13, OUTPUT);
  // pinMode(A13, INPUT);

  // statusLedLife = millis() + 2000;

  // Serial.println("trying");

  Serial.println("calling function");
  goclutch();
  return;

  while (!SD.begin(33, 18, 19, 5))
  {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card is inserted?");
    Serial.println("* Is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
  }

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("opening file to read...");
  logFile = SD.open("/log.txt", FILE_READ);

  output = getLastLineOfFile(logFile);
  Serial.println(output);

  logFile.close();
  Serial.println("done reading");

  Serial.println("last unique value");
  String lastUniqueValue = splitString(output, ',', 9);
  Serial.println(lastUniqueValue);

  // Does the file exist?
  String url = S3_BUCKET_URL + lastUniqueValue + ".txt";
  Serial.println("Seeing if url exists: " + url);
  http.begin(url);
  int httpCode = http.GET();
  http.end(); // Free up resources
  Serial.println("status code");
  Serial.println(httpCode);

  if (httpCode == 404)
  {
    // UDHttp udh;
    Serial.print("the heap size: ");
    Serial.println(ESP.getFreeHeap());

    // char bufftest[3];
    // Serial.print("sizeoftest");
    // Serial.println(sizeof(bufftest));
    // return;

    Serial.println("Not uploading, we are testing airtable now");
    HTTPClient airtableClient;
    airtableClient.begin(AIRTABLE_BASE_URL);
    airtableClient.addHeader("Authorization", AIRTABLE_KEY);
    airtableClient.addHeader("Content-Type", "application/json");

    int airtableResponseCode = airtableClient.POST(
        "{\
          \"fields\": { \
            \"Name\": \"" +
        lastUniqueValue + "\",\
            \"File\": [{\"filename\": \"log.txt\", \"url\": \"" +
        S3_BUCKET_URL + "LOG.TXT\"}]\
          }\
      }");
    Serial.print("airtable response: ");
    Serial.println(airtableResponseCode);
    Serial.println(airtableClient.getString());
    airtableClient.end();

    return;
    Serial.println("Time to upload");

    // logFile = SD.open("/sample.txt", FILE_READ);
    logFile = SD.open("/log.txt", FILE_READ);
    Serial.println("file opened");
    // uint8_t buffer2[logFile.size()];
    // while (logFile.available()) {
    //   // output = logFile.readStringUntil('\n');
    //   // Serial.println(output);
    //   // uint8_t buffer2[logFile.size()+ 1000];
    //   //logFile.readBytes(buffer2, logFile.size());

    //   buffer2[]
    // }
    // logFile.readBytes(buffer2, logFile.size());
    // char uniqueValueBuffer[50];
    // String temp = lastUniqueValue + ".txt";
    // temp.toCharArray(uniqueValueBuffer, 50);

    // udh.upload(AIRTABLE_BASE_URL, uniqueValueBuffer, logFile.size(), rdataf, progressf, responsef);
    // HTTPClient http2;
    // http2.begin("AIRTABLE_BASE_URL/ahhh6.txt");

    // WORKS!!!!!!
    // String myString = "hey im all kinds of data";
    // unsigned char buffer2[myString.length()];

    // myString.getBytes(buffer2, myString.length());
    // int resultCode = http2.PUT(buffer2, sizeof(buffer2));
    // Serial.println("done uploading file");
    // Serial.println(resultCode);
    // Serial.println("after result code");
    // WORKS!!!!

    // uint8_t buffer2[logFile.size()];
    // logFile.readBytes(buffer2, logFile.size());

    // myString.getBytes(buffer2, myString.length());
    // Serial.println(buffer2);
    // int resultCode = http2.PUT(buffer2, sizeof(buffer2));
    // Serial.println("done uploading file");
    // Serial.println(resultCode);
    // Serial.println("after result code");

    // UDHttp udh;
    // udh.upload(AIRTABLE_BASE_URL, "ahhh6.txt", logFile.size(), rdataf, progressf, responsef);

    WiFiClient client;
    // CircularBuffer<byte, 200000> bigBuffer;
    int maxSize = 200000;
    char bigBuffer[maxSize];
    int counter = 0;
    // unsigned long start = micros();
    Chrono chrono(Chrono::SECONDS);
    chrono.start();
    if (client.connect(AIRTABLE_BASE_URL, 80))
    {
      client.println(F("PUT /ahhh9.txt HTTP/1.1"));
      client.print(F("Host: "));
      client.println(AIRTABLE_BASE_URL);
      client.println(F("Connection: close"));
      client.print(F("Content-Length: "));
      client.println(logFile.size());
      client.println();
      // WORKS
      // while (logFile.available()) {
      //     client.write(logFile.read());

      // }
      // WORKS

      // while (logFile.available()) {
      //     // if (bigBuffer.isFull()) {
      //     //   Serial.println("Send it to the client");
      //     //   // client.write_P(bigBuffer.)
      //     //   for (byte i = 0; i < bigBuffer.size(); i++) {
      //     //     //client.write_P(bigBuffer);
      //     //     client.write(bigBuffer[i]);
      //     //   }
      //     //   bigBuffer.clear();
      //     // } else {
      //     //   // Add to the buffer
      //     //   bigBuffer.push(logFile.read());
      //     // }
      //     // if (sizeof(bigBuffer) < bigBufferSize) {
      //     //   // Add to buffer

      //     // }
      //     // We know the length of our big file, so if what we have left is less than 200000 then idk
      //     // Serial.println("0");
      //     // if (counter < maxSize) {
      //     //   Serial.println("1");
      //     //   bigBuffer[counter] = logFile.read();
      //     //   Serial.println("2");
      //     //   counter++;
      //     //   Serial.println("3");
      //     // } else {
      //     //   Serial.println("Buffer at capacity");
      //     //   client.write_P(bigBuffer, maxSize);
      //     //   counter = 0;
      //     // }

      //     // client.write(logFile.read());
      //     client.write(logFile);
      // }
      client.write(logFile);

      client.stop();

      while (client.available())
      {
        String line = client.readStringUntil('\r');
        Serial.print(line);
      }
      Serial.println("Done postint to s3");
    }
    else
    {
      Serial.println("Failed to connect for client");
    }

    chrono.stop();
    Serial.print("Upload took: ");
    Serial.print(chrono.elapsed());
    Serial.println(" seconds");

    logFile.close();
    Serial.println("done uploading");
    // http.PUT(buffer2, logFile.size());
  }
  else
  {
    Serial.println("Http code was other than 404");
  }
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
    for (int i = flen; i < len; ++i)
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
  for (int i = strlen(sz); i < len; ++i)
    sz[i] = ' ';
  if (len > 0)
    sz[len - 1] = ' ';
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
  for (int i = 0; i < len; ++i)
    Serial.print(i < slen ? str[i] : ' ');
  smartDelay(0);
}

void displayInfo()
{
  int voltage = analogRead(A13);

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

  printInt(voltage, true, 5);

  Serial.println();

  if (!gps.location.isValid())
  {
    Serial.println("Not persisting gps, lat/long is not valid");
    return;
  }

  if (gps.location.lat() == lastLatitude && gps.location.lng() == lastLongitude)
  {
    Serial.println("The latitude & longitude have not change, no reason to persist");
    return;
  }
  lastLatitude = gps.location.lat();
  lastLongitude = gps.location.lng();

  // Get our battery voltage
  //pinMode(13, INPUT);

  char buffer[1000];
  sprintf(buffer, "%ld,%0.2f,%f,%f,%ld,%02d/%02d/%02d,%02d:%02d:%02d,%f,%f,%ld,%ld",
          gps.satellites.value(), // 0
          gps.hdop.hdop(),        // 1
          gps.location.lat(),     // 2
          gps.location.lng(),     // 3
          gps.location.age(),     // 4

          gps.date.month(), // 5
          gps.date.day(),   // 5
          gps.date.year(),  // 5

          gps.time.hour(),   // 6
          gps.time.minute(), // 6
          gps.time.second(), // 6

          gps.altitude.meters(),   // 7
          gps.speed.mph(),         // 8
          millis() * esp_random(), // 9
          voltage                  // 10
  );

  Serial.println("trying to write to sd card");
  logFile = SD.open("/log.txt", FILE_WRITE);

  if (!logFile)
  {
    logFile.close();
  }

  logFile = SD.open("/log.txt", FILE_WRITE);
  Serial.println("writing to log.txt");
  logFile.println(String(buffer));
  logFile.close();
  Serial.println("done writing");
}
void loop()
{
  // while (MySerial.available() > 0)
  // {
  //   gps.encode(MySerial.read());
  // }

  // if (millis() < statusLedLife)
  // {
  //   digitalWrite(13, HIGH);
  // } else
  // {
  //   digitalWrite(13, LOW);
  // }

  // if (nextSerialTaskTs < millis())
  // {
  //   displayInfo();
  //   nextSerialTaskTs = millis() + TASK_SERIAL_RATE;
  // }
  if (nextSerialTaskTs < millis())
  {
    Serial.println("im here loop");
    nextSerialTaskTs = millis() + TASK_SERIAL_RATE;
  }
}