#ifndef WEBHELPER_H
#define WEBHELPER_H

// #include <HardwareSerial.h>
#include <WString.h>
// #include <mySD.h>

// String splitString(String data, char separator, int index);
// String getLastLineOfFile(File &file);
bool doesWebResourceExist(String url);

void uploadFile(File &file, String filename, char *url);

#endif