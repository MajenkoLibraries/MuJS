#include <MuJS.h>

// First create an interpreter object
MuJS js;

// This is the program we want to run
static const char *pgm = 

    "function setup() {"
    "   Serial.begin(115200);"
    "   Serial.println('I am now running a function that has been');"
    "   Serial.println('written in Javascript.');"
    "   pinMode(13, 1);"
    "}"
    ""
    "function loop() {"
    "   digitalWrite(13, 1);"
    "   delay(100);"
    "   digitalWrite(13, 0);"
    "   delay(900);"
    "   Serial.println(millis());"
    "}";



void setup() {
    // Initialize the interpreter
    js.begin();
    // Load the program
    js.load(pgm);
    // Call the setup function
    js.setup();
}

void loop() {
    // And repeatedly call the loop function
    js.loop();
}
