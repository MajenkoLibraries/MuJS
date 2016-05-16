#ifndef _MUJS_H
#define _MUJS_H

#include <Arduino.h>
#include <DSPI.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "utility/mujs.h"

#ifdef __cplusplus
}
#endif

class MuJS {
    private:
        js_State *J;
        void (*_logfunc)(const char *);

    public:
        MuJS() : _logfunc(NULL) {}
        void begin();
        void end();
        void load(const char *data);
        void setup();
        void loop();
        void call(const char *func);
        void abort();
        void onLog(void (*func)(const char *));
        void sendLog(const char *);
};

extern void js_serial_init(js_State *J);
extern void js_dspi_init(js_State *J);
extern void js_core_io_init(js_State *J);
extern void js_core_analog_init(js_State *J);
extern void js_core_time_init(js_State *J);
extern void js_core_sys_init(js_State *J);
extern void js_console_init(js_State *J, MuJS *);


#endif
