#include <WString.h>
#include <HTTPClient.h>

bool doesWebResourceExist(String url) {
    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();
    http.end(); // Free up resources

    return httpCode != 404;
}