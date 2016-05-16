#include <MuJS.h>

static void js_console_log(js_State *J) {
    const char *msg = js_tostring(J, 1);
    MuJS *mujs = (MuJS *)js_touserdata(J, 0, "MuJS");
    mujs->sendLog(msg);
    js_pushundefined(J);
}

static void js_console_new(js_State *J) {
    js_currentfunction(J);
    js_getproperty(J, -1, "prototype");
}

void js_console_init(js_State *J, MuJS *self) {
    js_getglobal(J, "Object");
    js_getproperty(J, -1, "prototype");

    js_newuserdata(J, "MuJS", self, 0);

    js_newcfunction(J, js_console_log, "log", 1);
    js_defproperty(J, -2, "log", JS_DONTENUM);
    
    js_newcconstructor(J, js_console_new, js_console_new, "Console", 1);
    js_defglobal(J, "Console", JS_DONTENUM);
    js_getglobal(J, "Console");

    js_construct(J, 0);
    js_defglobal(J, "console", JS_DONTENUM);
}

