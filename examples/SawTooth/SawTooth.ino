#include <MuJS.h>

// First create an interpreter object
MuJS js;

// This is the program we want to run
static const char *pgm = 

    "function setup() {"
    "   DSPI0.begin();"
    "   DSPI0.setSpeed(20000000);"
    "   pinMode(10, OUTPUT);"
    "}"
    ""
    "var dacval = 0;"
    ""
    "function loop() {"
    "   digitalWrite(10, LOW);"
    "   DSPI0.transfer(0x30 | ((dacval >> 8) & 0x0F));"
    "   DSPI0.transfer(dacval & 0xFF);"
    "   digitalWrite(10, HIGH);"
    "   dacval += 16;"
    "   while (dacval > 4095) {"
    "       dacval -= 4095;"
    "   }"
    "}";

// Uncomment this for the Hardware Accelerated SD library
// #define USE_HW_SD

void setup() {
    // Initialize the interpreter
    js.begin();
    // Load the program
//    loadFile(js, "test.js");
    js.load(pgm);
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
