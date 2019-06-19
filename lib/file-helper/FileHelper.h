#ifndef FILEHELPER_H
#define FILEHELPER_H

// #include <HardwareSerial.h>
#include <WString.h>
#include <mySD.h>

// String splitString(String data, char separator, int index);
String getLastLineOfFile(File &file);

#endif