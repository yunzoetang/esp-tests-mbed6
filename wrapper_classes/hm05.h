#include "mbed.h"

void runHM05Test(instance_data_t *data) {
    LED *redLED = data->redLED;
    BufferedSerial *pc = data->pc;
    BufferedSerial *hm10 = data->hm10;
    C12832 *lcd = data->lcd;

    // Application buffer to receive the data
    char buf[32] = {0};

    while (!data->exit_requested) {
        if (hm10->readable()) {
            uint32_t num = hm10->read(buf, sizeof(buf));
            lcd->write(buf, num);
        }
}