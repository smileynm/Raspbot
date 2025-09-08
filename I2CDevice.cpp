#include "I2CDevice.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <cstdio>

I2CDevice::I2CDevice(int bus, int addr) {
    char filename[20];
    snprintf(filename, 19, "/dev/i2c-%d", bus);
    fd = open(filename, O_RDWR);
    if (fd < 0)
        perror("open");
    if (ioctl(fd, I2C_SLAVE, addr) < 0)
        perror("ioctl");
}

I2CDevice::~I2CDevice() {
    if (fd >= 0)
        close(fd);
}

bool I2CDevice::writeData(const unsigned char *data, int len) {
    return (write(fd, data, len) == len);
}

bool I2CDevice::readData(unsigned char reg, unsigned char *buf, int len) {
    if (write(fd, &reg, 1) != 1)
        return false;
    return (read(fd, buf, len) == len);
}