#include <MuJS.h>
#include <SD.h>

// First create an interpreter object
MuJS js;

// Uncomment this for the Hardware Accelerated SD library
// #define USE_HW_SD

#ifdef USE_HW_SD
DSPI2 sdspi;
Sd2Card myCard&spi);
SDClass mySD(myCard);
#else
#define mySD SD
#endif

void initSD() {
    pinMode(52, OUTPUT);
    digitalWrite(52, HIGH);
    if (!mySD.begin(52)) {
        Serial.begin(115200);
        Serial.println("Error configuring SD card");
    }
}

void loadFile(MuJS &J, const char *file) {
    File f = SD.open(file);
    if (!f) {
        Serial.begin(115200);
        Serial.print("Unable to open file ");
        Serial.println(file);
        return;
    }

    int len = f.available();
    char *text = (char *)alloca(len+1);
    text[len] = 0;
    f.readBytes(text, f.available());
    J.load(text);
}

void setup() {
    // Start the SD card
    initSD();
    // Initialize the interpreter
    js.begin();
    // Load the program
    loadFile(js, "test.js");
//    js.load(pgm);
    // Call the setup function
    js.setup();
}

void loop() {
    // And repeatedly call the loop function
    js.loop();
}

extern "C" {

#include <sys/time.h>
#include <wiring.h>
#include <stdint.h>

int gettimeofday(struct timeval *tv, void *tz) {
    uint32_t m = millis();
    tv->tv_sec = m / 1000;
    tv->tv_usec = (m % 1000) * 1000;
    return 0;
}

}
