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

    public:
        void begin();
        void end();
        void load(const char *data);
        void setup();
        void loop();
        void call(const char *func);
        void abort();
};

extern void js_serial_init(js_State *J);
extern void js_dspi_init(js_State *J);
extern void js_core_io_init(js_State *J);
extern void js_core_time_init(js_State *J);
extern void js_core_sys_init(js_State *J);


#endif
