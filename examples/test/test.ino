#include <MuJS.h>

static const char *pgm = "function run() { println('I am now running a function that has been'); println('written in Javascript.'); }";

static void print(js_State *J) {
    Serial.print(js_tostring(J, 1));
    js_pushundefined(J);
}

static void println(js_State *J) {
    Serial.println(js_tostring(J, 1));
    js_pushundefined(J);
}

js_State *JS;

void setup() {

    Serial.begin(115200);
    
    JS = js_newstate(NULL, NULL, JS_STRICT);
    js_newcfunction(JS, print, "print", 1);
    js_setglobal(JS, "print");
    js_newcfunction(JS, println, "println", 1);
    js_setglobal(JS, "println");

    js_dostring(JS, pgm);

    js_dostring(JS, "run();");
}

void loop() {
    
}