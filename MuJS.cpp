#include <MuJS.h>

// We don't need to do anything in here at the moment.
// Eventually we will have a nice OO wrapper.

void MuJS::begin() {
    J = js_newstate(NULL, NULL, JS_STRICT);
    js_core_io_init(J);
    js_core_time_init(J);
    js_serial_init(J);
}

void MuJS::load(const char *data) {
    js_dostring(J, data);
}

void MuJS::setup() {
    js_getglobal(J, "setup");
    js_pushnull(J);
    js_call(J, 0);
}

void MuJS::loop() {
    js_getglobal(J, "loop");
    js_pushnull(J);
    js_call(J, 0);
}
