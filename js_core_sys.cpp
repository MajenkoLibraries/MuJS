#include <MuJS.h>

static void js_core_sys_yield(js_State *J) {
    _scheduleTask();
    js_pushundefined(J);
}

void js_core_sys_init(js_State *J) {
    js_newcfunction(J, js_core_sys_yield, "yield", 2);
    js_setglobal(J, "yield");
}
