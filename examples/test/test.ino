#include <MuJS.h>

// First create an interpreter object
MuJS js;

// This is the program we want to run
static const char *pgm = 

    "var s = new Serial(0);"
    "function setup() {"
    "   s.begin(115200);"
    "   s.println('I am now running a function that has been');"
    "   s.println('written in Javascript.');"
    "   pinMode(13, 1);"
    "}"
    ""
    "function loop() {"
    "   digitalWrite(13, 1);"
    "   delay(100);"
    "   digitalWrite(13, 0);"
    "   delay(900);"
    "   s.println(millis());"
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
