#include "os.h"

extern deviceCB_t *serial;

void drivers_init() {
    serial_init();
    device_open(serial, FLAG_INT_RX);
}