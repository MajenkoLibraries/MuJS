#include <MuJS.h>

static void js_dspi_begin(js_State *J) {
    DSPI *port = (DSPI *)js_touserdata(J, 0, "Port");
    port->begin();
    js_pushundefined(J);
}

static void js_dspi_transfer(js_State *J) {
    DSPI *port = (DSPI *)js_touserdata(J, 0, "Port");
    js_pushnumber(J, port->transfer((uint8_t)js_tonumber(J, 1)));
}

static void js_dspi_new(js_State *J) {
    int port = js_tonumber(J, 1);
    js_currentfunction(J);
    js_getproperty(J, -1, "prototype");

    switch (port) {
#if (NUM_DSPI_PORTS > 0)
        case 0:
            js_newuserdata(J, "Port", new DSPI0, NULL);
            break;
#endif

#if (NUM_DSPI_PORTS > 1)
        case 1:
            js_newuserdata(J, "Port", new DSPI1, NULL);
            break;
#endif

#if (NUM_DSPI_PORTS > 2)
        case 2:
            js_newuserdata(J, "Port", new DSPI2, NULL);
            break;
#endif

#if (NUM_DSPI_PORTS > 3)
        case 3:
            js_newuserdata(J, "Port", new DSPI3, NULL);
            break;
#endif

    }
}

void js_dspi_init(js_State *J) {
    js_getglobal(J, "Object");
    js_getproperty(J, -1, "prototype");
    js_newuserdata(J, "Port", 0, 0);

    js_newcfunction(J, js_dspi_begin, "begin", 0);
    js_defproperty(J, -2, "begin", JS_DONTENUM);
    js_newcfunction(J, js_dspi_transfer, "transfer", 2);
    js_defproperty(J, -2, "transfer", JS_DONTENUM);
    
    js_newcconstructor(J, js_dspi_new, js_dspi_new, "DSPI", 1);
    js_defglobal(J, "DSPI", JS_DONTENUM);

#if (NUM_DSPI_PORTS > 0)
    js_getglobal(J, "DSPI");
    js_pushnumber(J, 0);
    js_construct(J, 1);
    js_defglobal(J, "DSPI0", JS_DONTENUM);
#endif

#if (NUM_DSPI_PORTS > 1)
    js_getglobal(J, "DSPI");
    js_pushnumber(J, 1);
    js_construct(J, 1);
    js_defglobal(J, "DSPI1", JS_DONTENUM);
#endif

#if (NUM_DSPI_PORTS > 2)
    js_getglobal(J, "DSPI");
    js_pushnumber(J, 2);
    js_construct(J, 1);
    js_defglobal(J, "DSPI2", JS_DONTENUM);
#endif

#if (NUM_DSPI_PORTS > 3)
    js_getglobal(J, "DSPI");
    js_pushnumber(J, 3);
    js_construct(J, 1);
    js_defglobal(J, "DSPI3", JS_DONTENUM);
#endif


}

