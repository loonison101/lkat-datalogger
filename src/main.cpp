#include <HardwareSerial.h>
#include <TinyGPS++.h>
#include <SPI.h>
#include <mySD.h>
#include <WiFi.h>

Sd2Card card;
SdVolume volume;
SdFile root;
File logFile;

const int chipSelect = 33;

HardwareSerial MySerial(1);
TinyGPSPlus gps;
uint32_t nextSerialTaskTs = 0;
#define TASK_SERIAL_RATE 1000 * 1

double lastLatitude = 0;
double lastLongitude = 0;

// For 2 seconds show the LED so we know we are up and running
uint32_t statusLedLife;

void setup()
{
  Serial.begin(115200);
  MySerial.begin(9600, SERIAL_8N1, 16, 17);

  WiFi.mode(WIFI_OFF);
  btStop();

  pinMode(chipSelect, OUTPUT);
  pinMode(13, OUTPUT);

  statusLedLife = millis() + 2000; 

  Serial.println("trying");

  while (!SD.begin(33, 18, 19, 5))
  {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card is inserted?");
    Serial.println("* Is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
  }

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

  if (!gps.location.isValid())
  {
    Serial.println("Not persisting gps, lat/long is not valid");
    return;
  }

  if (gps.location.lat() == lastLatitude && gps.location.lng() == lastLongitude) {
    Serial.println("The latitude & longitude have not change, no reason to persist");
    return;
  }
  lastLatitude = gps.location.lat();
  lastLongitude = gps.location.lng();

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

  if (logFile)
  {
    Serial.println("writing to log.txt");
    logFile.println(String(buffer));
    logFile.close();
    Serial.println("done writing");
  }
  else
  {
    Serial.println("error opening log.txt");
  }
}
void loop()
{
  while (MySerial.available() > 0)
  {
    gps.encode(MySerial.read());
  }

  // while (MySerial.available() > 0) {
  //     uint8_t byteFromSerial = MySerial.read();
  //     Serial.write(byteFromSerial);
  // }

  if (millis() < statusLedLife) {
    digitalWrite(13, HIGH);
  } else {
    digitalWrite(13, LOW);
  }

  if (nextSerialTaskTs < millis())
  {

    displayInfo();

    nextSerialTaskTs = millis() + TASK_SERIAL_RATE;
  }
}