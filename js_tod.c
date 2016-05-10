#include <sys/time.h>
#include <wiring.h>
#include <stdint.h>

int gettimeofday(struct timeval *tv, void *tz) {
    uint32_t m = millis();
    tv->tv_sec = m / 1000;
    tv->tv_usec = (m % 1000) * 1000;
    return 0;
}

