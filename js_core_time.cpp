#include <MuJS.h>


static void js_core_time_delay(js_State *J) {
    uint32_t t = js_tonumber(J, 1);
    delay(t);
    js_pushundefined(J);
}

static void js_core_time_millis(js_State *J) {
    js_pushnumber(J, millis());
}

void js_core_time_init(js_State *J) {
    js_newcfunction(J, js_core_time_delay, "delay", 1);
    js_setglobal(J, "delay");

    js_newcfunction(J, js_core_time_millis, "millis", 0);
    js_setglobal(J, "millis");
}
