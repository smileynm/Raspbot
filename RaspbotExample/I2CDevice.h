#pragma once
#include <string>

class I2CDevice {
public:
    I2CDevice(int bus, int addr);
    ~I2CDevice();
    bool writeData(const unsigned char *data, int len);
    bool readData(unsigned char reg, unsigned char *buf, int len);

private:
    int fd;
};