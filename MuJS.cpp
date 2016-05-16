#include <MuJS.h>

// We don't need to do anything in here at the moment.
// Eventually we will have a nice OO wrapper.

static void js_dopanic(js_State *J) {
    Serial.begin(115200);
    Serial.printf("uncaught exception: %s\n", js_tostring(J, -1));
    js_abort(J);
}

void MuJS::begin() {
    J = js_newstate(NULL, NULL, JS_STRICT);
    js_atpanic(J, js_dopanic);
    js_core_sys_init(J);
    js_core_io_init(J);
    js_core_analog_init(J);
    js_core_time_init(J);
    js_serial_init(J);
    js_dspi_init(J);
    js_console_init(J, this);
}

void MuJS::end() {
    js_freestate(J);
}

void MuJS::load(const char *data) {
//    if (js_try(J)) {
//        Serial.println("Compilation Error");
//        if (J->panic) {
//            J->panic(J);
//        }
//        js_pop(J, 1);
//        return;
//    }
    js_loadstring(J, "[string]", data);
    if (!js_iscallable(J, 0)) {
        Serial.println("Compilation Error");
        return;
    }
    js_pushglobal(J);
    js_call(J, 0);
    js_pop(J, 1);
//    js_endtry(J);
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
    js_call(J, 0);
    js_pop(J, 1);
    js_gc(J, 0);
}

void MuJS::abort() {
    js_abort(J);
}

void MuJS::onLog(void (*func)(const char *)) {
    _logfunc = func;
}

void MuJS::sendLog(const char *s) {
    if (_logfunc != NULL) {
        _logfunc(s);
    }
}

extern "C" {
void argh(const char *m) {
Serial.println(m);
}
}
