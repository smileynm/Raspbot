#include "RaspbotService.h"
#include <iostream>

RaspbotService::RaspbotService(I2CController& i2cController) : i2c(i2cController) {
}

// 모터 제어
bool RaspbotService::controlMotor(MotorNumber motor, MotorDirection direction, unsigned char speed) {
    std::vector<unsigned char> data = {static_cast<unsigned char>(motor),
                                       static_cast<unsigned char>(direction), speed};
    return i2c.writeData(RaspbotReg::MOTOR_SPEED, data);
}

// 서보 제어
bool RaspbotService::controlServo(unsigned char servoNumber, unsigned char angle) {
    if (servoNumber < 1 || servoNumber > 2 || angle > 180) {
        std::cerr << "경고: 잘못된 서보 번호(1-2) 또는 각도(0-180)." << std::endl;
        return false;
    }
    std::vector<unsigned char> data = {servoNumber, angle};
    return i2c.writeData(RaspbotReg::SERVO, data);
}

// RGB LED 제어 (전체 고정 밝기)
bool RaspbotService::controlRgbAll(DeviceStatus status, RgbColor color) {
    std::vector<unsigned char> data;
    if (status == DeviceStatus::OFF) {
        data = {static_cast<unsigned char>(DeviceStatus::OFF), 0x00};   // 색상 비활성화
    } else {
        data = {static_cast<unsigned char>(DeviceStatus::ON), static_cast<unsigned char>(color)};
    }
    return i2c.writeData(RaspbotReg::RGB_ALL_FIXED_BRIGHTNESS, data);
}


bool RaspbotService::controlRgbIndividual(unsigned char ledNumber, DeviceStatus status, RgbColor color) {
    if (ledNumber < 1 || ledNumber > 14) {
        std::cerr << "경고: 잘못된 LED 번호(1-14)." << std::endl;
        return false;
    }
    std::vector<unsigned char> data;
    if (status == DeviceStatus::OFF) {
        data = {ledNumber, static_cast<unsigned char>(DeviceStatus::OFF), 0x00}; // 색상 비활성화
    } else {
        data = {ledNumber, static_cast<unsigned char>(DeviceStatus::ON), static_cast<unsigned char>(color)};
    }
    return i2c.writeData(RaspbotReg::RGB_INDIVIDUAL_FIXED_BRIGHTNESS, data);
}

// RGB LED 제어 (색상 조정)
bool RaspbotService::setRgbAllBrightness(unsigned char r, unsigned char g, unsigned char b) {
    std::vector<unsigned char> data = {r, g, b};
    return i2c.writeData(RaspbotReg::RGB_ALL_BRIGHTNESS, data);
}

bool RaspbotService::setRgbIndividualBrightness(unsigned char ledNumber, unsigned char r, unsigned char g, unsigned char b) {
    if (ledNumber < 1 || ledNumber > 14) {
        std::cerr << "경고: 잘못된 LED 번호(1-14)입니다." << std::endl;
        return false;
    }
    std::vector<unsigned char> data = {ledNumber, r, g, b};
    return i2c.writeData(RaspbotReg::RGB_INDIVIDUAL_BRIGHTNESS, data);
}

// 부저 제어
bool RaspbotService::controlBuzzer(DeviceStatus status) {
    std::vector<unsigned char> data = {static_cast<unsigned char>(status)};
    return i2c.writeData(RaspbotReg::BUZZER, data);
}

// 초음파 센서 제어
bool RaspbotService::controlUltrasonic(DeviceStatus status) {
    std::vector<unsigned char> data = {static_cast<unsigned char>(status)};
    return i2c.writeData(RaspbotReg::ULTRASONIC_CONTROL_SWITCH, data);
}

std::pair<bool, int> RaspbotService::readUltrasonicDistance() {
    std::vector<unsigned char> low_data = i2c.readData(RaspbotReg::ULTRASONIC_DATA_LOW_BIT, 1);
    std::vector<unsigned char> high_data = i2c.readData(RaspbotReg::ULTRASONIC_DATA_HIGH_BIT, 1);

    if (low_data.empty() || high_data.empty()) {
        std::cerr << "오류: 초음파 센서 데이터 읽기 실패." << std::endl;
        return std::make_pair(false, 0);
    }

    int distance = (high_data[0] << 8) | low_data[0];
    return std::make_pair(true, distance);
}

std::pair<bool, unsigned char> RaspbotService::readInfraredSensorData() {
    std::vector<unsigned char> data = i2c.readData(RaspbotReg::IR_SENSOR_DATA, 1);
    if (data.empty()) {
        std::cerr << "오류: 적외선 센서 데이터 읽기 실패." << std::endl;
        return std::make_pair(false, 0);
    }
    return std::make_pair(true, data[0]);
}

std::pair<bool, unsigned char> RaspbotService::readInfraredCodeValue() {
    std::vector<unsigned char> data = i2c.readData(RaspbotReg::IR_CODE_VALUE_DATA, 1);
    if (data.empty()) {
        std::cerr << "오류: 적외선 코드 데이터 읽기 실패." << std::endl;
        return std::make_pair(false, 0);
    }
    return std::make_pair(true, data[0]);
}

// Key Data
std::pair<bool, unsigned char> RaspbotService::readKeyData() {
    std::vector<unsigned char> data = i2c.readData(RaspbotReg::KEY_DATA, 1);
    if (data.empty()) {
        std::cerr << "오류: 키 데이터 읽기 실패." << std::endl;
        return std::make_pair(false, 0);
    }
    return std::make_pair(true, data[0]);
}