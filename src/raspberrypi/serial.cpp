#include "serial.h"

//#define UART_BASE 0x20201000 // PL011 USART -->B+
#define UART_BASE 0x3F201000 // PL011 USART (PI2B+)
#define BLOCK_SIZE 4096

void Serial::setBaudRate()
{
    int fd = 0;
    int mem_fd = 0;
    int errsv = 0;

    volatile unsigned *uart = NULL;
    void *uart_map = NULL;
    void *IBRD = NULL;
    void *FBRD = NULL;
    void *LCRH = NULL;
    void *UARTCR = NULL;
    void *UARTFR = NULL;

    uint32_t brd = 0;

    struct termios termios_s; // save for restore
    struct termios termios_p;

    fd = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NONBLOCK);

    if (fd > 0) // set the configuration for the terminal
    {
        tcgetattr(fd, &termios_s);

        cfmakeraw(&termios_p);

        termios_p.c_lflag &= ~ISIG;      // no signals
        termios_p.c_lflag &= ~ICANON;   // no canonical mode
        termios_p.c_lflag &= ~ECHO;   // no echo input
        termios_p.c_lflag &= ~NOFLSH;   // no flushing on SIGINT
        termios_p.c_lflag &= ~IEXTEN;   // no input processing

        termios_p.c_cc[VMIN] = 0;
        termios_p.c_cc[VTIME] = 0;

        tcsetattr(fd, TCSADRAIN, &termios_p);

        // enable the mmap
        if ((mem_fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0)
        {
            fprintf(stdout, "can't open /dev/mem for mmap(). Did you use 'sudo' ?\n");
            return -1;
        }

        uart_map = mmap(NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, UART_BASE);

        close(mem_fd);

        if (uart_map == MAP_FAILED)
        {
            errsv = errno;
            fprintf(stdout, "uart mmap failed\n");
            fprintf(stdout, "error %d %s\n", errsv, strerror(errsv));
            return -1;
        }
        else
        {
            uart = (volatile unsigned *) uart_map;
            UARTFR = (void *) ((uint32_t) uart + 0x18);
            IBRD = (void *) ((uint32_t) uart + 0x24);
            FBRD = (void *) ((uint32_t) uart + 0x28);
            LCRH = (void *) ((uint32_t) uart + 0x2C);
            UARTCR = (void *) ((uint32_t) uart + 0x30);

            // first, disable the uart and flush the fifos
            brd = 0x00;
            memcpy(UARTCR, &brd, 4);

            // check for completion of any tx/rx
            memcpy(&brd, UARTFR, 4);
            while (brd & 0x08)
            {
                usleep(1); // wait a bit
                memcpy(&brd, UARTFR, 4);
            }

            // midi
            brd = 6; // 3000000 / ( 16 * 31250 ) = 6.0
            memcpy(IBRD, &brd, 4);
            brd = 0;
            memcpy(FBRD, &brd, 4);
            brd = 0x70; // 8 bit data, FIFO enabled
            memcpy(LCRH, &brd, 4);

            // enable uart, tex, rex etc.
            brd = 0x0301;
            memcpy(UARTCR, &brd, 4);
        }
    }
    else
    {
        errsv = errno;
        fprintf(stdout, "/dev/serial0 access failed\n");
        fprintf(stdout, "error %d %s\n", errsv, strerror(errsv));
        fd = 0;
        return -1;
    }

    return fd;
}

int Serial::rate_to_constant(int baudrate)
{
#define B(x) case x: return B##x
    switch (baudrate)
    {
        B(50);
        B(75);
        B(110);
        B(134);
        B(150);
        B(200);
        B(300);
        B(600);
        B(1200);
        B(1800);
        B(2400);
        B(4800);
        B(9600);
        B(19200);
        B(38400);
        B(57600);
        B(115200);
        B(230400);
        B(460800);
        B(500000);
        B(576000);
        B(921600);
        B(1000000);
        B(1152000);
        B(1500000);
        default:
            return 0;
    }
#undef B
}

int Serial::serialOpen(const char *device, int rate)
{
    struct termios options;
    struct serial_struct serinfo;
    int fd;
    int speed = 0;

    if ((fd = open(device, CS8 | O_RDWR | O_NOCTTY)) == -1)
    {
        printf("Opening file descriptor failed.\n");
        return -1;
    }

    speed = rate2constant(rate);

    if (speed == 0)
    {
        serinfo.reserved_char[0] = 0;

        if (ioctl(fd, TIOCGSERIAL, &serinfo) < 0)
        {
            printf("ioctl failed.\n");
            return -1;
        }

        serinfo.flags &= ~ASYNC_SPD_MASK;
        serinfo.flags |= ASYNC_SPD_CUST;
        serinfo.custom_divisor = (serinfo.baud_base + (rate / 2)) / rate;
        printf("Baud base: %d.\n", serinfo.baud_base);
        printf("Custom divisor: %d.\n", serinfo.custom_divisor);

        if (serinfo.custom_divisor < 1)
        {
            printf("Custom divisor set to 1.\n");
            serinfo.custom_divisor = 1;
        }

        if (ioctl(fd, TIOCSSERIAL, &serinfo) < 0)
        {
            printf("ioctl failed.\n");
            return -1;
        }

        if (ioctl(fd, TIOCGSERIAL, &serinfo) < 0)
        {
            printf("ioctl failed.\n");
            return -1;
        }

        if (serinfo.custom_divisor * rate != serinfo.baud_base)
        {
            printf("actual baudrate is %d / %d = %f\n",
                   serinfo.baud_base, serinfo.custom_divisor,
                   (float) serinfo.baud_base / serinfo.custom_divisor);
        }
    }

    fcntl(fd, F_SETFL, 0);
    tcgetattr(fd, &options);
    cfsetispeed(&options, speed ?: B38400);
    cfsetospeed(&options, speed ?: B38400);
    cfmakeraw(&options);
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~CRTSCTS;

    if (tcsetattr(fd, TCSANOW, &options) != 0)
    {
        printf("tcsetattr failed.\n");
        return -1;
    }

    return fd;
}
