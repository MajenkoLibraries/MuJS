#include <MuJS.h>

class SerialWrapper : public Stream {
    private:
        HardwareSerial *_UART;
#ifdef _USE_USB_FOR_SERIAL_
        USBSerial *_USB;
#endif
        Stream *_stream;
    public:
        SerialWrapper(HardwareSerial &hw) {
            _stream = &hw;
            _UART = &hw;
#ifdef _USE_USB_FOR_SERIAL_
            _USB = NULL;
#endif            
        }
#ifdef _USE_USB_FOR_SERIAL_
        SerialWrapper(USBSerial &hw) {
            _stream = &hw;
            _UART = NULL;
            _USB = &hw;
        }
#endif            

        void begin(uint32_t br) {
            if (_UART != NULL) {
                _UART->begin(br);
#ifdef _USE_USB_FOR_SERIAL_
            } else if (_USB != NULL) {
                _USB->begin(br);
#endif
            }
        }

        size_t write(uint8_t c) { return _stream->write(c); }
        int read() { return _stream->read(); }
        int available() { return _stream->available(); }
        int peek() { return _stream->peek(); }
        void flush() { _stream->flush(); }
};

static void js_serial_begin(js_State *J) {
    SerialWrapper *port = (SerialWrapper *)js_touserdata(J, 0, "Port");
    int br = (int)js_tonumber(J, 1);
    port->begin(br);
    js_pushundefined(J);
}

static void js_serial_read(js_State *J) {
    SerialWrapper *port = (SerialWrapper *)js_touserdata(J, 0, "Port");
    js_pushnumber(J, port->read());
}

static void js_serial_peek(js_State *J) {
    SerialWrapper *port = (SerialWrapper *)js_touserdata(J, 0, "Port");
    js_pushnumber(J, port->peek());
}

static void js_serial_available(js_State *J) {
    SerialWrapper *port = (SerialWrapper *)js_touserdata(J, 0, "Port");
    js_pushnumber(J, port->available());
}

static void js_serial_print(js_State *J) {
    SerialWrapper *port = (SerialWrapper *)js_touserdata(J, 0, "Port");
  
    if (js_isnumber(J, 1)) {
        if (js_isundefined(J, 2)) {
            port->print(js_tonumber(J, 1));
        } else {
            port->print(js_tonumber(J, 1), js_tonumber(J, 2));
        }
    } else if (js_isstring(J, 1)) {
        port->print(js_tostring(J, 1));
    } else if (js_isboolean(J, 1)) {
        port->print(js_toboolean(J, 1) ? "1" : "0");
    }
    js_pushundefined(J);
}

static void js_serial_println(js_State *J) {
    SerialWrapper *port = (SerialWrapper *)js_touserdata(J, 0, "Port");
  
    if (js_isnumber(J, 1)) {
        if (js_isundefined(J, 2)) {
            port->println(js_tonumber(J, 1));
        } else {
            port->println(js_tonumber(J, 1), js_tonumber(J, 2));
        }
    } else if (js_isstring(J, 1)) {
        port->println(js_tostring(J, 1));
    } else if (js_isboolean(J, 1)) {
        port->println(js_toboolean(J, 1) ? "1" : "0");
    }
    js_pushundefined(J);
}

static void js_serial_new(js_State *J) {
    int port = js_tonumber(J, 1);
    js_currentfunction(J);
    js_getproperty(J, -1, "prototype");

    switch (port) {
#ifdef _USE_USB_FOR_SERIAL_
            js_newuserdata(J, "Port", new SerialWrapper(Serial), NULL);
        case -1:
#endif
        case 0:
#ifdef _USE_USB_FOR_SERIAL_
            js_newuserdata(J, "Port", new SerialWrapper(Serial0), NULL);
#else
            js_newuserdata(J, "Port", new SerialWrapper(Serial), NULL);
#endif
            break;

#if (NUM_SERIAL_PORTS > 1)
        case 1:
            js_newuserdata(J, "Port", new SerialWrapper(Serial1), NULL);
            break;
#endif

#if (NUM_SERIAL_PORTS > 2)
        case 2:
            js_newuserdata(J, "Port", new SerialWrapper(Serial2), NULL);
            break;
#endif

#if (NUM_SERIAL_PORTS > 3)
        case 3:
            js_newuserdata(J, "Port", new SerialWrapper(Serial3), NULL);
            break;
#endif

#if (NUM_SERIAL_PORTS > 4)
        case 4:
            js_newuserdata(J, "Port", new SerialWrapper(Serial4), NULL);
            break;
#endif

#if (NUM_SERIAL_PORTS > 5)
        case 5:
            js_newuserdata(J, "Port", new SerialWrapper(Serial5), NULL);
            break;
#endif

    }
}

void js_serial_init(js_State *J) {
    js_getglobal(J, "Object");
    js_getproperty(J, -1, "prototype");
    js_newuserdata(J, "Port", 0, 0);

    js_newcfunction(J, js_serial_begin, "begin", 1);
    js_defproperty(J, -2, "begin", JS_DONTENUM);
    js_newcfunction(J, js_serial_print, "print", 2);
    js_defproperty(J, -2, "print", JS_DONTENUM);
    js_newcfunction(J, js_serial_println, "println", 2);
    js_defproperty(J, -2, "println", JS_DONTENUM);
    js_newcfunction(J, js_serial_read, "read", 0);    
    js_defproperty(J, -2, "read", JS_DONTENUM);
    js_newcfunction(J, js_serial_peek, "read", 0);    
    js_defproperty(J, -2, "peek", JS_DONTENUM);
    js_newcfunction(J, js_serial_available, "available", 0);    
    js_defproperty(J, -2, "available", JS_DONTENUM);
    
    js_newcconstructor(J, js_serial_new, js_serial_new, "SerialDevice", 1);
    js_defglobal(J, "SerialDevice", JS_DONTENUM);

    js_getglobal(J, "SerialDevice");
#ifdef _USE_USB_FOR_SERIAL_
    js_pushnumber(J, -1); // USB serial port
    js_construct(J, 1);
    js_defglobal(J, "Serial", JS_DONTENUM);
    js_pushnumber(J, 0); // UART 0 serial port
    js_construct(J, 1);
    js_defglobal(J, "Serial0", JS_DONTENUM);
#else
    js_pushnumber(J, 0); // UART 0 serial port
    js_construct(J, 1);
    js_defglobal(J, "Serial", JS_DONTENUM);
#endif

#if (NUM_SERIAL_PORTS > 1)
    js_pushnumber(J, 1); // UART 1 serial port
    js_construct(J, 1);
    js_defglobal(J, "Serial1", JS_DONTENUM);
#endif

#if (NUM_SERIAL_PORTS > 2)
    js_pushnumber(J, 2); // UART 2 serial port
    js_construct(J, 1);
    js_defglobal(J, "Serial2", JS_DONTENUM);
#endif

#if (NUM_SERIAL_PORTS > 3)
    js_pushnumber(J, 3); // UART 3 serial port
    js_construct(J, 1);
    js_defglobal(J, "Serial3", JS_DONTENUM);
#endif

#if (NUM_SERIAL_PORTS > 4)
    js_pushnumber(J, 4); // UART 4 serial port
    js_construct(J, 1);
    js_defglobal(J, "Serial4", JS_DONTENUM);
#endif

#if (NUM_SERIAL_PORTS > 5)
    js_pushnumber(J, 5); // UART 5 serial port
    js_construct(J, 1);
    js_defglobal(J, "Serial5", JS_DONTENUM);
#endif

}

