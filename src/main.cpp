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
#include <WebHelper.h>
#include <AirtableHelper.h>
#include <HTTPClient.h>

/*
  SHIFT + OPTION + F - formatl all code
 */

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

// Every time we start this device it's helpful to know all the gps points are correlated
// during a power on session. So you could hike 4 times in a day, and easily know you were
// in 4 seperate sessions in a day.
uint32_t sessionId;

HTTPClient http;

static void turnOffWifi() {
  WiFi.mode(WIFI_OFF);
  btStop();
}

void setup()
{
  Serial.begin(115200);

  Serial.println("Starting up...");
  MySerial.begin(9600, SERIAL_8N1, 16, 17);

  pinMode(chipSelect, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(A13, INPUT);

  // Show LED that we are starting up
  digitalWrite(13, HIGH);

  // Whether we have wifi or not, we need SD card for the loop
  while (!SD.begin(33, 18, 19, 5))
  {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card is inserted?");
    Serial.println("* Is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
  }

  // Do we want to upload anything?
  if (!SHOULD_UPLOAD_DATA) {
    Serial.print("You do not want to upload anything");
    turnOffWifi();
    digitalWrite(13, LOW);
    return;
  }

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int wifiConnectAttempts = 0;
  while (WiFi.status() != WL_CONNECTED && wifiConnectAttempts <= WIFI_CONNECT_TRY_COUNT)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
    wifiConnectAttempts++;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Couldn't connect to: "); Serial.println(WIFI_SSID);
    turnOffWifi();
    digitalWrite(13, LOW);
    return;
  }

  Serial.print("WiFi connected, IP address: ");
  Serial.println(WiFi.localIP());

  logFile = SD.open("/log.txt", FILE_READ);

  String output;
  output = getLastLineOfFile(logFile);
  Serial.print("Last line of file: ");
  Serial.println(output);
  logFile.close();
  
  if (output.indexOf("Uuid")) {
    Serial.println("No data in csv file, not uploading");
    turnOffWifi();
    digitalWrite(13, LOW);
    return;
  }

  String lastUniqueValue = splitString(output, ',', 9);
  String newFilenameWithExtension = lastUniqueValue + ".txt";
  Serial.print("New filename with extension: ");
  Serial.println(newFilenameWithExtension);

  // Does the file exist?
  String slash = "/";
  String urlWithFile = S3_BUCKET_URL + slash + newFilenameWithExtension;
  Serial.print("Url with filename: ");Serial.println(urlWithFile);

  bool s3FileExists = doesWebResourceExist(urlWithFile);

  if (s3FileExists)
  {
    Serial.println("S3 file already exists");
    turnOffWifi();
    digitalWrite(13, LOW);
    return;
  }

  Serial.println("Uploading to S3...");
  logFile = SD.open("/log.txt", FILE_READ);
  
  Chrono chrono(Chrono::SECONDS);
  chrono.start();
  uploadFile(logFile, newFilenameWithExtension, S3_BUCKET_URL);
  chrono.stop();
  Serial.print("Upload took: ");
  Serial.print(chrono.elapsed());
  Serial.println(" seconds");
  logFile.close();
  Serial.println("done uploading");

  AirtableHelper airtableHelper(AIRTABLE_BASE_URL, AIRTABLE_KEY);
  airtableHelper.createAirtableRecord(newFilenameWithExtension, urlWithFile);
  

  Serial.println("Everything done, turning off wifi");
  turnOffWifi();
  digitalWrite(13, LOW);
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

// You can't compare doubles with == due to rounding errors
// https://stackoverflow.com/questions/18971533/c-comparison-of-two-double-values-not-working-properly?lq=1
bool double_equals(double a, double b, double epsilon = 0.001)
{
    return std::abs(a - b) < epsilon;
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

  if (double_equals(gps.location.lat(), lastLatitude) 
        && double_equals(gps.location.lng(), lastLongitude))
  {
    Serial.println("The latitude & longitude have not change, no reason to persist");
    return;
  }
  lastLatitude = gps.location.lat();
  lastLongitude = gps.location.lng();

  // Get our battery voltage
  //pinMode(13, INPUT);

  char buffer[1000];
  sprintf(buffer, "%ld,%0.2f,%f,%f,%ld,%02d/%02d/%02d,%02d:%02d:%02d,%f,%f,%ld,%ld,%d",
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

          gps.altitude.meters(),  // 7
          gps.speed.mph(),        // 8
          millis() * esp_random(),// 9
          voltage,                // 10
          sessionId               // 11
  );

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
  while (MySerial.available() > 0)
  {
    gps.encode(MySerial.read());
  }

  if (nextSerialTaskTs < millis())
  {
    displayInfo();
    nextSerialTaskTs = millis() + TASK_SERIAL_RATE;
  }
  if (nextSerialTaskTs < millis())
  {
    nextSerialTaskTs = millis() + TASK_SERIAL_RATE;
  }
}