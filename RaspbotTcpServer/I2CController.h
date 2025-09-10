#ifndef I2CCONTROLLER_H
#define I2CCONTROLLER_H

#include <string>
#include <vector>

class I2CController {
public:
    I2CController(int deviceAddress, const std::string& devicePath = "/dev/i2c-1");
    ~I2CController();

    bool openDevice();
    void closeDevice();

    // I2C 데이터 쓰기 (레지스터 주소, 데이터 버퍼, 길이)
    bool writeData(unsigned char reg, const std::vector<unsigned char>& data);

    // I2C 데이터 읽기 (레지스터 주소, 읽을 길이)
    std::vector<unsigned char> readData(unsigned char reg, int length);

private:
    int i2c_fd;
    int deviceAddress;
    std::string devicePath;
    bool isOpened;
};

#endif // I2CCONTROLLER_H