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

    js_pushnumber(J, 0);
    js_setglobal(J, "INPUT");

    js_pushnumber(J, 1);
    js_setglobal(J, "OUTPUT");

    js_pushnumber(J, 2);
    js_setglobal(J, "OPEN");

    js_pushnumber(J, 3);
    js_setglobal(J, "INPUT_PULLUP");

    js_pushnumber(J, 4);
    js_setglobal(J, "INPUT_PULLDOWN");

    js_pushnumber(J, 5);
    js_setglobal(J, "INPUT_PULLUPDOWN");

    js_pushnumber(J, 0);
    js_setglobal(J, "LOW");

    js_pushnumber(J, 1);
    js_setglobal(J, "HIGH");

    js_pushnumber(J, 1);
    js_setglobal(J, "CHANGE");

    js_pushnumber(J, 2);
    js_setglobal(J, "FALLING");

    js_pushnumber(J, 3);
    js_setglobal(J, "RISING");

#ifdef PIN_LED1
    js_pushnumber(J, PIN_LED1);
    js_setglobal(J, "PIN_LED1");
#endif
#ifdef PIN_LED2
    js_pushnumber(J, PIN_LED2);
    js_setglobal(J, "PIN_LED2");
#endif
#ifdef PIN_LED3
    js_pushnumber(J, PIN_LED3);
    js_setglobal(J, "PIN_LED3");
#endif
#ifdef PIN_LED4
    js_pushnumber(J, PIN_LED4);
    js_setglobal(J, "PIN_LED4");
#endif
#ifdef A0
    js_pushnumber(J, A0);
    js_setglobal(J, "A0");
#endif
#ifdef A1
    js_pushnumber(J, A1);
    js_setglobal(J, "A1");
#endif
#ifdef A2
    js_pushnumber(J, A2);
    js_setglobal(J, "A2");
#endif
#ifdef A3
    js_pushnumber(J, A3);
    js_setglobal(J, "A3");
#endif
#ifdef A4
    js_pushnumber(J, A4);
    js_setglobal(J, "A4");
#endif
#ifdef A5
    js_pushnumber(J, A5);
    js_setglobal(J, "A5");
#endif
#ifdef A6
    js_pushnumber(J, A6);
    js_setglobal(J, "A6");
#endif
#ifdef A7
    js_pushnumber(J, A7);
    js_setglobal(J, "A7");
#endif
#ifdef A8
    js_pushnumber(J, A8);
    js_setglobal(J, "A8");
#endif
#ifdef A9
    js_pushnumber(J, A9);
    js_setglobal(J, "A9");
#endif
#ifdef A10
    js_pushnumber(J, A10);
    js_setglobal(J, "A10");
#endif
#ifdef A11
    js_pushnumber(J, A11);
    js_setglobal(J, "A11");
#endif
#ifdef A12
    js_pushnumber(J, A12);
    js_setglobal(J, "A12");
#endif
#ifdef A13
    js_pushnumber(J, A13);
    js_setglobal(J, "A13");
#endif
#ifdef A14
    js_pushnumber(J, A14);
    js_setglobal(J, "A14");
#endif
#ifdef A15
    js_pushnumber(J, A15);
    js_setglobal(J, "A15");
#endif

}
