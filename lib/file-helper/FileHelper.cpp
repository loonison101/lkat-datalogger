#include <WString.h>
#include <mySD.h>

String getLastLineOfFile(File &file) {
    String output;
    if (file.size() > 1000) {
        int seekTo = file.size() - 1000;
        file.seek(seekTo);
    }
    
    while (file.available()) {
        output = file.readStringUntil('\n');
    }

    return output;
}