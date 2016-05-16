#include <MuJS.h>
#include <CircularBuffer.h>

#include <DFATFS.h>
#include <DSDVOL.h>
#include <ROMVOL.h>
#include <DSPI.h>
#include <MRF24G.h>          
#include <DEIPcK.h>
#include <DEWFcK.h>


// Start of network configuration

IPv4 ip = {192,168,0,205};
IPv4 gateway = {192,168,0,1};
IPv4 netmask = {255,255,255,0};
uint16_t port = 80;
const char *ssid = "Your SSID";
const char *psk = "Your PSK";

// End of network configuration


extern const uint32_t _binary_objects_fs_dat_start;
extern const uint32_t _binary_objects_fs_dat_end;
extern const uint32_t _binary_objects_fs_dat_size;

ROMVOL vol((const uint8_t *)&_binary_objects_fs_dat_start, ((uint32_t)&_binary_objects_fs_dat_size) / _MIN_SS + 1);

DefineSDSPI(dSDSpi);
DefineDSDVOL(sd, dSDSpi);

// First create an interpreter object
MuJS js;
CircularBuffer<char> logBuffer(4096);

bool reloadProgram = false;
bool isSDMounted = false;

// This is the program we want to run
static const char *pgm = 

    "function setup() {\n"
    "    pinMode(PIN_LED2, OUTPUT);\n"
    "}\n"
    "\n"
    "function loop() {\n"
    "    digitalWrite(PIN_LED2, HIGH);\n"
    "    delay(100);\n"
    "    digitalWrite(PIN_LED2, LOW);\n"
    "    delay(900);\n"
    "}\n";
    
    static char program[131072];

static const char spinner[4] = {'/', '-', '\\', '|'};

TCPServer webServer;

struct webclient {
    TCPSocket socket;
    char buffer[1024];
    char page[50];
    uint32_t bpos;
    uint32_t body;
    uint32_t flags;
#define F_CLOSED 0
#define F_CONNECTED 1
#define F_GET 2
#define F_POST 4
};

#define CLIENTS 10

struct webclient client[CLIENTS];

struct webpage {
    const char *path;
    void (*function)(webclient *c);
};

static inline bool endsWith(const char *str, const char *suffix) {
    int stringLength = strlen(str);
    int suffixLength = strlen(suffix);
    int suffixStart = stringLength - suffixLength;
    if (strncmp(&str[suffixStart], suffix, suffixLength) == 0) {
        return true;
    }
    return false;
}

bool sendFile(const char *filename, TCPSocket *s, const char *mime = NULL) {

    if (!s->isConnected()) {
        return false;
    }
    
    char gzip[strlen(filename) + 4];
    sprintf(gzip, "%s.gz", filename);

    Serial.print("Looking for ");
    Serial.print(filename);
    Serial.print(" or ");
    Serial.print(gzip);
    Serial.print("...");

    bool isGZIP = false;
    
    if (!DFATFS::fsexists(gzip)) {
        if (!DFATFS::fsexists(filename)) {
            Serial.println("It is not found");
            return false;
        }
        Serial.println("It is plain");
    } else {
        Serial.println("It is GZIP");
        isGZIP = true;
    }

    DFILE file;
    if (isGZIP) {
        FRESULT fr = file.fsopen(gzip, FA_READ);
        if (fr != FR_OK) {
            return false;
        }
    
        if (mime != NULL) { // This wants a header
            s->println("HTTP/1.0 200 OK");
            s->println("Connection: close");
            s->print("Content-type: ");
            s->println(mime);
            s->println("Content-encoding: gzip");
            s->print("Content-length: ");
            s->println(file.fssize());
            s->println("Server: Wi-Fire Javascript Editor");
            s->println("Last-Modified: Sat, 13 Octr 2015 19:14:25 GMT");
            s->println();
        }
    } else {
        FRESULT fr = file.fsopen(filename, FA_READ);
        if (fr != FR_OK) {
            return false;        
        }
    
        if (mime != NULL) { // This wants a header
            s->println("HTTP/1.0 200 OK");
            s->println("Connection: close");
            s->print("Content-type: ");
            s->println(mime);
            s->print("Content-length: ");
            s->println(file.fssize());
            s->println("Server: Wi-Fire Javascript Editor");
            s->println("Last-Modified: Sat, 13 Octr 2015 19:14:25 GMT");
            s->println();
        }
    }

    const int chunksize = DFILE::FS_DEFAULT_BUFF_SIZE;
        
    uint8_t temp[chunksize];
    uint32_t l = 0;
    uint32_t toread = file.fssize();
    while (toread > 0) {
        uint32_t thisread = min(toread, chunksize);
        
        file.fsread(temp, thisread, &l);

        toread -= thisread;
        Serial.printf("Asked for %d, got %d. %d left.\r\n", thisread, l, toread);

        uint32_t towrite = thisread;

        while (towrite > 0) {
            if (!s->isConnected()) {
                file.fsclose();    
                return false;
            }
            uint32_t numwritten = s->writeStream(&temp[thisread - towrite], towrite);
            towrite -= numwritten;
            DEIPcK::periodicTasks();
        }
        
    }
    file.fsclose();
    return true;
}

void CSS_file(webclient *c) {
    if (!sendFile(c->page, &c->socket, "text/css")) {        
        notFound(&c->socket);
        return;
    }
}

static char hex2dec(unsigned char hex) {
    if(hex > 0x39) hex -= 7; // adjust for hex letters upper or lower case
    return(hex & 0xf);
}

void POST_setcode(webclient *c) {
    int pgmptr = 0;
    int charcount = 0;

#define SC_KEY 0
#define SC_BODY 1
#define SC_PCTA 2
#define SC_PCTB 3

    int state = SC_KEY;
  
    while (charcount < c->body) {
        DEIPcK::periodicTasks();

        if (c->socket.available()) {
            charcount++;
            char ch = c->socket.read();

            if (ch == '=' && state == SC_KEY) {
                state = SC_BODY;
            } else if (ch == '+' && state == SC_BODY) {
                program[pgmptr++] = ' ';
                program[pgmptr] = 0;
            } else if (ch == '%' && state == SC_BODY) {
                state = SC_PCTA;
            } else if (state == SC_PCTA) {
                program[pgmptr] = hex2dec(ch) << 4;
                state = SC_PCTB;
            } else if (state == SC_PCTB) {
                program[pgmptr] |= hex2dec(ch);
                pgmptr++;
                program[pgmptr] = 0;
                state = SC_BODY;
            } else if (state == SC_BODY) {
                program[pgmptr++] = ch;
                program[pgmptr] = 0;
            }
        }
    }
    c->socket.println("HTTP/1.0 302 Moved");
    c->socket.println("Location: /index.html");
    c->socket.println();
    js.abort();
    reloadProgram = true;
}

void redirect_index(webclient *c) {
    c->socket.println("HTTP/1.0 301 Redirected Permanantly");
    c->socket.println("Location: /index.html");
    c->socket.println();
}

void GET_getcode(webclient *c) {
    c->socket.println("HTTP/1.0 200 OK");
    c->socket.println("Content-type: text/plain");
    c->socket.println("Connection: close");
    c->socket.print("Content-length: ");
    c->socket.println(strlen(program));
    c->socket.println("Server: Wi-Fire Javascript Editor");
    c->socket.println();
    c->socket.print(program);
}

void GET_getfiles(webclient *c) {
    c->socket.println("HTTP/1.0 200 OK");
    c->socket.println("Content-type: text/html");
    c->socket.println("Connection: close");
    c->socket.println("Server: Wi-Fire Javascript Editor");
    c->socket.println();

    if (isSDMounted == false) {
        c->socket.println("<h4>SD card is not mounted</h4>");
        c->socket.println("<button class='mount' onClick='mountSD();'/>Mount SD card</button>");
    } else {
        DIR dp;
        static FILINFO fi;
        char temp[256];
        fi.lfname = temp;
        fi.lfsize = 256;
        if (f_opendir(&dp, "1:/") != FR_OK) {
            Serial.println("Unable to get directory listing");
            c->socket.println("Error getting directory listing");
            return;
        }

        c->socket.println("<ul>");

        while (f_readdir(&dp, &fi) == FR_OK) {
            if (fi.fname[0] == 0) break;
            if (endsWith(fi.lfname, ".ckj")) {
                c->socket.print("<li>");
                c->socket.print("<a href='/openFile.ckj?file=");
                c->socket.print(fi.lfname);
                c->socket.print("'>");
                c->socket.print(fi.lfname);
                c->socket.println("</a></li>");
            }
            DEIPcK::periodicTasks();
        }
        f_closedir(&dp);
        Serial.println("Directory done");
        c->socket.println("</ul>");
        c->socket.println("<button class='unmount' onClick='unmountSD();'/>Unmount SD card</button>");        
    }
}

void GET_mount(webclient *c) {
    c->socket.println("HTTP/1.0 200 OK");
    c->socket.println("Content-type: application/json");
    c->socket.println("Connection: close");
    c->socket.println("Server: Wi-Fire Javascript Editor");
    c->socket.println();

    if (isSDMounted == false) {

        // Attempt to mount the SD here...
        FRESULT fr = DFATFS::fsmount(sd, DFATFS::szFatFsVols[1], 0);
        if (fr == FR_OK) {
            c->socket.print("{\"mounted\":true, \"error\":false}");
            isSDMounted = true;
        } else {
            c->socket.print("{\"mounted\":false, \"error\":true, \"message\":\"Error mounting SD card\"}");
        }
        return;
    }
    c->socket.print("{\"mounted\":true, \"error\":true, \"message\":\"SD card is already mounted\"}");
}

void GET_unmount(webclient *c) {
    c->socket.println("HTTP/1.0 200 OK");
    c->socket.println("Content-type: application/json");
    c->socket.println("Connection: close");
    c->socket.println("Server: Wi-Fire Javascript Editor");
    c->socket.println();

    if (isSDMounted == true) {

        // Attempt to unmount the SD here...
        FRESULT fr = DFATFS::fsunmount(DFATFS::szFatFsVols[1]);
        if (fr == FR_OK) {        
            c->socket.print("{\"mounted\":false, \"error\":false}");
            isSDMounted = false;
        } else {
            c->socket.print("{\"mounted\":true, \"error\":true, \"message\":\"Error unmounting SD card\"}");
        }
        return;
    }
    c->socket.print("{\"mounted\":false, \"error\":true, \"message\":\"SD card is not mounted\"}");
}

void GET_log(webclient *c) {
    uint32_t len = logBuffer.available();
    c->socket.println("HTTP/1.0 200 OK");
    c->socket.println("Content-type: application/json");
    c->socket.println("Connection: close");
    c->socket.print("Content-length: ");
    c->socket.println(len);
    c->socket.println("Server: Wi-Fire Javascript Editor");
    c->socket.println();
    for (int i = 0; i < len; i++) {
        c->socket.write(logBuffer.read());
    }
}

struct webpage getpages[] = {
    {"/", redirect_index},
    {"/getCode.ck", GET_getcode},
    {"/getFiles.ck", GET_getfiles},
    {"/mount.ck", GET_mount},
    {"/unmount.ck", GET_unmount},
    {"/getLog.ck", GET_log},
    {0, 0}
};

struct webpage postpages[] = {
    {"/setCode.ck", POST_setcode},
    {0, 0}
};

//TCPSocket client[5];
IPSTATUS status;





void notFound(TCPSocket *c) {
    c->println("HTTP/1.0 403 Not Found");
    c->println("Connection: close");
    c->println("Content-type: text/html");
    c->println();
    c->println("<h1>Not Found</h1>");
}

void processGet(webclient *c) {

    Serial.print("GET [");
    Serial.print(c->page);
    Serial.println("]");
    for (int i = 0; getpages[i].path != 0; i++) {
        if (!strcmp(getpages[i].path, c->page)) {
            getpages[i].function(c);
            return;
        }
    }

    Serial.println("Not an indexed page. Looking for a file.");

    if (endsWith(c->page, ".html")) {
        if (!sendFile(c->page, &c->socket, "text/html")) {
            notFound(&c->socket);        
        }
        return;
    }

    if (endsWith(c->page, ".js")) {
        if (!sendFile(c->page, &c->socket, "text/javascript")) {
            notFound(&c->socket);        
        }
        return;
    }

    if (endsWith(c->page, ".css")) {
        if (!sendFile(c->page, &c->socket, "text/css")) {
            notFound(&c->socket);        
        }
        return;
    }

    if (endsWith(c->page, ".ico")) {
        if (!sendFile(c->page, &c->socket, "image/x-icon")) {
            notFound(&c->socket);        
        }
        return;
    }

    if (endsWith(c->page, ".png")) {
        if (!sendFile(c->page, &c->socket, "image/png")) {
            notFound(&c->socket);        
        }
        return;
    }

    if (endsWith(c->page, ".jpg")) {
        if (!sendFile(c->page, &c->socket, "image/jpeg")) {
            notFound(&c->socket);        
        }
        return;
    }

    
    notFound(&c->socket);        
}

void processPost(webclient *c) {
    for (int i = 0; postpages[i].path != 0; i++) {
        if (!strcmp(postpages[i].path, c->page)) {
            postpages[i].function(c);
            return;
        }
    }
    notFound(&c->socket);    
}

void processClient(int cno) {
    if (client[cno].socket.isConnected()) {
        while (client[cno].socket.available() > 0) {
            char c = client[cno].socket.read();
            switch(c) {
                case '\r': break;
                case '\n':
                    Serial.printf("[%s]\r\n", client[cno].buffer);
                    if (strncmp(client[cno].buffer, "GET ", 4) == 0) {
                        char *type = strtok(client[cno].buffer, " ");
                        char *page = strtok(NULL, " ");
                        strcpy(client[cno].page, page);
                        client[cno].flags |= F_GET;
                    } else if (strncmp(client[cno].buffer, "POST ", 5) == 0) {
                        char *type = strtok(client[cno].buffer, " ");
                        char *page = strtok(NULL, " ");
                        client[cno].body = 0;
                        strcpy(client[cno].page, page);
                        client[cno].flags |= F_POST;
                    } else if (strncasecmp(client[cno].buffer, "Content-Length: ", 16) == 0) {
                        char *hn = strtok(client[cno].buffer, " ");
                        char *bl = strtok(NULL, " ");
                        if (bl) {
                            client[cno].body = atoi(bl);
                        }
                    } else if (strlen(client[cno].buffer) == 0) {
                        if (client[cno].flags & F_GET) {
                            processGet(&(client[cno]));
                        } else if (client[cno].flags & F_POST) {
                            processPost(&(client[cno]));
                        } else {
                            notFound(&(client[cno].socket));
                        }
                        client[cno].socket.close();
                        client[cno].flags = F_CLOSED;
                        webServer.addSocket(client[cno].socket);
                    }
                    client[cno].bpos = 0;
                    client[cno].buffer[0] = 0;
                    break;
                default:
                    if (client[cno].bpos < 1023) {
                        client[cno].buffer[client[cno].bpos++] = c;
                        client[cno].buffer[client[cno].bpos] = 0;
                    }
                    break;
                    
            }
        }
    } else {
        client[cno].flags = F_CLOSED;
        webServer.addSocket(client[cno].socket);
        Serial.println("Client gone!");
    }
}

void startWorking(TCPSocket *cli) {
    for (int i = 0; i < CLIENTS; i++) {
        if (&(client[i].socket) == cli) {
            client[i].buffer[0] = 0;
            client[i].bpos = 0;
            client[i].flags = F_CONNECTED;
        }
    }
}

void runNetwork(int taskid, void *tptr) {
    DEIPcK::periodicTasks();

    if (webServer.isListening() > 0) {
        if (webServer.availableClients() > 0) {
            IPEndPoint ep;
            Serial.println("Client connected");            
            TCPSocket *c = webServer.acceptClient();
            c->getRemoteEndPoint(ep);
            Serial.printf("[%d.%d.%d.%d]\r\n", 
                ep.ip.ipv4.u8[0],
                ep.ip.ipv4.u8[1],
                ep.ip.ipv4.u8[2],
                ep.ip.ipv4.u8[3]);
            startWorking(c);
        }
    }

    for (int z = 0; z < CLIENTS; z++) {
        if (client[z].flags & F_CONNECTED) {
            processClient(z);
        }
    }
}

void getLog(const char *s) {
    for (const char *p = s; *p; p++) {
        logBuffer.write(*p);
    }
    logBuffer.write('\n');
}

void setup() {
    strcpy(program, pgm);
    Serial.begin(115200);

    Serial.println();
    Serial.print("Flash FS size: 0x");
    Serial.println((uint32_t)&_binary_objects_fs_dat_size, HEX);

    Serial.print("Flash FS start address: 0x");
    Serial.println((uint32_t)&_binary_objects_fs_dat_start, HEX);
    
    Serial.println("Mounting Flash Volume...");

    FRESULT fr = DFATFS::fsmount(vol, DFATFS::szFatFsVols[0], 0);
    if (fr != FR_OK) {
        Serial.println("Unable to mount flash filesystem");
    }

    Serial.print("Drive ");
    Serial.print(DFATFS::szFatFsVols[0]);
    Serial.println(" mounted!");

    Serial.println("Done.");

    js.onLog(getLog);
    // Initialize the interpreter
    js.begin();
    // Load the program
    js.load(pgm);

    int i = 0;
    Serial.print("Connecting to WiFi...  ");
    while (!deIPcK.wfConnect(ssid, psk, &status)) {
        Serial.print((char)8);
        Serial.print(spinner[i]);
        i++;
        i %= 4;
        
        if (IsIPStatusAnError(status)) {
            Serial.print("Cannot connect WiFi: ");
            Serial.println(status, DEC);
            while(1);
        }

        DEIPcK::periodicTasks();
        
    }
    Serial.print((char)8);
    Serial.println("Connected");

    deIPcK.begin(ip, gateway, netmask);
    Serial.println("Started IP");
    if (!deIPcK.tcpStartListening(port, webServer)) {
        Serial.println("Error listening");
    } else {
        Serial.println("Listening");
        for (int z = 0; z < CLIENTS; z++) {
            webServer.addSocket(client[z].socket);
        }
    }
    Serial.println("Server ready");

    // Call the setup function
    js.setup();
    Serial.println("Setup executed");
    Serial.print("Running...  ");

    createTask(runNetwork, 1, TASK_ENABLE, NULL);
}


void loop() {
    static int i = 0;

   
    // And repeatedly call the loop function
    js.loop();

    if (reloadProgram) {
        reloadProgram = false;
        js.end();
        js.begin();
        js.load(program);
        js.setup();
    }
    
}

#if 1
extern "C" {

#include <sys/time.h>
#include <wiring.h>
#include <stdint.h>

int gettimeofday(struct timeval *tv, void *tz) {
    uint32_t m = millis();
    tv->tv_sec = m / 1000;
    tv->tv_usec = (m % 1000) * 1000;
    return 0;
}

void xxdebug(const char *c) {
    Serial.print("DEBUG: ");
    Serial.println(c);
}

}
#endif
