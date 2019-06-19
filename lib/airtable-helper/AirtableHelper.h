#ifndef AIRTABLEHELPER_H
#define AIRTABLEHELPER_H

// #include <HardwareSerial.h>
#include <WString.h>
// #include <mySD.h>
// #include <HTTPClient.h>

// String splitString(String data, char separator, int index);
// String getLastLineOfFile(File &file);
// bool doesWebResourceExist(String url);
class AirtableHelper {
    private:
        String _baseUrl;
        String _key;
    public:
        AirtableHelper(String baseUrl, String key);

        String createAirtableRecord(String fileName, String fileUrl);
};

#endif