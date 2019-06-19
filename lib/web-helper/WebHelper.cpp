#include <WString.h>
#include <HTTPClient.h>
#include <mySD.h>
#include <WiFi.h>
#include "WebHelper.h"

bool doesWebResourceExist(String url) {
    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();
    http.end(); // Free up resources

    return httpCode != 404;
}

// Good person: https://forum.arduino.cc/index.php?topic=556337.msg3793750#msg3793750
void uploadFile(File &file, String filename, char *url) {
    String strippedUrl = String(url);
    strippedUrl.replace("http://", "");
    strippedUrl.replace("https://", "");
    Serial.print("stripped: ");
    Serial.println(strippedUrl);

    WiFiClient client;
    Serial.println("wifi upload s3");
    Serial.println(filename);
    Serial.println(strippedUrl);
    Serial.println("size of file");
    Serial.println(file.size());

    if (client.connect(url, 80))
    {
      String putString = "PUT /"+filename+" HTTP/1.1";
      client.println(F(putString.c_str()));
      client.print(F("Host: "));
      client.println(strippedUrl);
      client.println(F("Connection: close"));
      client.print(F("Content-Length: "));
      client.println(file.size());
      client.println();
      
      client.write(file);

      Serial.println("what happneed wiht the send off?");
      while (client.available())
      {
        String line = client.readStringUntil('\r');
        Serial.print(line);
      }
      client.stop();
      Serial.println("Done postint to s3");
    }
    else
    {
      Serial.println("Failed to connect for client");
    }
    
}