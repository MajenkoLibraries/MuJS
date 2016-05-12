#include <MuJS.h>

// We don't need to do anything in here at the moment.
// Eventually we will have a nice OO wrapper.

void js_dopanic(js_State *J) {
    Serial.printf("uncaught exception: %s\n", js_tostring(J, -1));
}

void MuJS::begin() {
    J = js_newstate(NULL, NULL, JS_STRICT);
    js_atpanic(J, js_dopanic);
    js_core_sys_init(J);
    js_core_io_init(J);
    js_core_time_init(J);
    js_serial_init(J);
    js_dspi_init(J);
}

void MuJS::end() {
    js_freestate(J);
}

void MuJS::load(const char *data) {
    js_dostring(J, data);
}

void MuJS::setup() {
    call("setup");
}

void MuJS::loop() {
    call("loop");
}

void MuJS::call(const char *func) {
    js_getglobal(J, func);
    js_pushnull(J);
    js_pcall(J, 0);
    js_pop(J, 1);
    js_gc(J, 0);
}

void MuJS::abort() {
    js_abort(J);
}
