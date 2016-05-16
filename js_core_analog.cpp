#include <MuJS.h>

static void js_core_analog_analogRead(js_State *J) {
    int pin = js_tonumber(J, 1);
    js_pushnumber(J, analogRead(pin));
}

static void js_core_analog_analogWrite(js_State *J) {
    int pin = js_tonumber(J, 1);
    int val = js_tonumber(J, 2);
    analogWrite(pin, val);
    js_pushundefined(J);
}

void js_core_analog_init(js_State *J) {
    js_newcfunction(J, js_core_analog_analogRead, "analogRead", 1);
    js_setglobal(J, "analogRead");

    js_newcfunction(J, js_core_analog_analogWrite, "analogWrite", 2);
    js_setglobal(J, "analogWrite");
}
