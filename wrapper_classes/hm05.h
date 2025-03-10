#include "mbed.h"
#include "Menu.h"
#include "C12832.h"

extern Menu menu;
extern BufferedSerial hm10, pc;
extern C12832 lcd;

void runHM05Test() {
    // Application buffer to receive the data
    char buf[32] = {0};

    while (!menu.exitRequested()) {
        if (hm10.readable()) {
            uint32_t num = hm10.read(buf, sizeof(buf));
            lcd.write(buf, num);
        }
}