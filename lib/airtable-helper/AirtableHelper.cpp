#include <WString.h>
#include <HTTPClient.h>
#include "AirtableHelper.h";

AirtableHelper::AirtableHelper(String baseUrl, String key) {
    _baseUrl = baseUrl;
    _key = key;
}

String AirtableHelper::createAirtableRecord(String fileName, String fileUrl) {
    HTTPClient airtableClient;
    airtableClient.begin(_baseUrl);
    airtableClient.addHeader("Authorization", _key);
    airtableClient.addHeader("Content-Type", "application/json");

    int airtableResponseCode = airtableClient.POST(
        "{\
          \"fields\": { \
            \"Name\": \"" +
        fileName + "\",\
            \"File\": [{\"filename\": \"log.txt\", \"url\": \""+fileUrl+"\"}]\
          }\
      }");
    String output = airtableClient.getString();
    airtableClient.end();
    return output;
}