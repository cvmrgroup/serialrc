#ifndef PI_SERIAL_H
#define PI_SERIAL_H

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/serial.h>

class Serial
{
public:
    static int rate2constant(int baudrate);
    static int serialOpen(const char *device, int rate);
    static int serialClose(int fd);
};

#endif//PI_SERIAL_H
