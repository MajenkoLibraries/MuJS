#include <MuJS.h>

static void js_core_io_pinMode(js_State *J) {
    int pin = js_tonumber(J, 1);
    int mode = js_tonumber(J, 2);
    pinMode(pin, mode);
    js_pushundefined(J);
}

static void js_core_io_digitalWrite(js_State *J) {
    int pin = js_tonumber(J, 1);
    int mode = js_tonumber(J, 2);
    digitalWrite(pin, mode);
    js_pushundefined(J);
}

static void js_core_io_digitalRead(js_State *J) {
    int pin = js_tonumber(J, 1);
    js_pushnumber(J, digitalRead(pin));
}

void js_core_io_init(js_State *J) {
    js_newcfunction(J, js_core_io_pinMode, "pinMode", 2);
    js_setglobal(J, "pinMode");

    js_newcfunction(J, js_core_io_digitalWrite, "digitalWrite", 2);
    js_setglobal(J, "digitalWrite");

    js_newcfunction(J, js_core_io_digitalRead, "digitalRead", 1);
    js_setglobal(J, "digitalRead");
}
