#ifndef PI_SERIAL_H
#define PI_SERIAL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/serial.h>

class Serial
{
public:
    static int serialOpen(const char *device, int rate);
    static int rate_to_constant(int baudrate);
};

#endif//PI_SERIAL_H
