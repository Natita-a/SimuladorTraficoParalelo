#include <threads.h>
#include <time.h>

void sleep_ms(int ms) {
    struct timespec ts = {.tv_sec = ms / 1000,
                          .tv_nsec = (ms % 1000) * 1000000L};
    thrd_sleep(&ts, NULL);
}
