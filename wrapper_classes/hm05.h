#include "mbed.h"

void runHM05Test() {
    // Application buffer to receive the data
    char buf[32] = {0};

    while (!menu.exitRequest()) {
        if (hm10.readable()) {
            uint32_t num = hm10.read(buf, sizeof(buf));
            lcd.write(buf, num);
        }
}