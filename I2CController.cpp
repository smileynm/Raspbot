#include "I2CController.h"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <cstring>

I2CController::I2CController(int deviceAddress, const std::string& devicePath)
    : i2c_fd(-1), deviceAddress(deviceAddress), devicePath(devicePath), isOpened(false) {
}

I2CController::~I2CController() {
    if (isOpened) {
        closeDevice();
    }
}

bool I2CController::openDevice() {
    i2c_fd = open(devicePath.c_str(), O_RDWR);
    if (i2c_fd < 0) {
        std::cerr << "오류: I2C 장치" << devicePath << "를 열 수 없음: " << strerror(errno) << std::endl;
        return false;
    }

    if (ioctl(i2c_fd, I2C_SLAVE, deviceAddress) < 0) {
        std::cerr << "오류: I2C 주소 설정 실패: 0x" << std::hex << deviceAddress << std::endl;
        close(i2c_fd);
        i2c_fd = -1;
        return false;
    }

    isOpened = true;
    std::cout << "I2C 장치 0x" << std::hex << deviceAddress << " 연결됨." << std::endl;
    return true;
}

void I2CController::closeDevice() {
    if (i2c_fd != -1) {
        close(i2c_fd);
        i2c_fd = -1;
        isOpened = false;
        std::cout << "I2C 장치 0x" << std::hex << deviceAddress << " 연결 해제됨." << std::endl;
    }
}

bool I2CController::writeData(unsigned char reg, const std::vector<unsigned char>& data) {
    if (!isOpened) {
        std::cerr << "오류: I2C 장치가 열려 있지 않음." << std::endl;
        return false;
    }

    std::vector<unsigned char> buf;
    buf.push_back(reg);                                 // 첫 바이트는 레지스터 주소
    buf.insert(buf.end(), data.begin(), data.end());    // 그 다음 데이터

    if (write(i2c_fd, buf.data(), buf.size()) != static_cast<ssize_t>(buf.size())) {
        std::cerr << "오류: 12C 쓰기 실패, 레지스터 0x" << std::hex << (int)reg << std::endl;
        return false;
    }

    usleep(50000);  // 50ms 지연 (Raspbot 프로토콜 요구사항)
    return true;
}

std::vector<unsigned char> I2CController::readData(unsigned char reg, int length) {
    if (!isOpened) {
        std::cerr << "오류: I2C 장치가 열려있지 않음" << std::endl;
        return {};
    }

    // 읽기 전, 읽으려는 레지스터 주소를 설정
    if (write(i2c_fd, &reg, 1) != 1) {
        std::cerr << "오류: 레지스터 주소 설정 실패, 레지스터 0x" << std::hex << (int)reg << std::endl;
        return {};
    }

    std::vector<unsigned char> readBuf(length);
    if (read(i2c_fd, readBuf.data(), length) != length) {
        std::cerr << "오류: I2C 읽기 실패, 레지스터 0x" << std::hex << (int)reg << std::endl;
        return {};
    }

    return readBuf;
}