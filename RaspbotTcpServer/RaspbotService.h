#ifndef RASPBOTSERVICE_H
#define RASPBOTSERVICE_H

#include "I2CController.h"
#include "RaspbotProtocol.h"
#include <vector>
#include <string>
#include <utility>

class RaspbotService {
public:
    RaspbotService(I2CController& i2cController);

    // 모터 제어
    bool controlMotor(MotorNumber motor, MotorDirection direction, unsigned char speed);

    // 서보 제어
    bool controlServo(unsigned char servoNumber, unsigned char angle); // 1-2, 0-180도

    // RGB LED 제어 (전체 고정 밝기)
    bool controlRgbAll(DeviceStatus status, RgbColor color);
    bool controlRgbIndividual(unsigned char ledNumber, DeviceStatus status, RgbColor color); // 1-14

    // RGB LED 제어 (전체 밝기 조절)
    bool setRgbAllBrightness(unsigned char r, unsigned char g, unsigned char b);
    bool setRgbIndividualBrightness(unsigned char ledNumber, unsigned char r, unsigned char g, unsigned char b); // 1-14

    // 부저 제어
    bool controlBuzzer(DeviceStatus status);

    // 초음파 센서 제어
    bool controlUltrasonic(DeviceStatus status);
    std::pair<bool, int> readUltrasonicDistance(); // (성공여부, 거리cm)
    
    // 적외선 센서 제어
    bool controlInfraredSensor(DeviceStatus status);
    std::pair<bool, unsigned char> readInfraredSensorData();    // 적외선 센서 (라인트레이싱)
    std::pair<bool, unsigned char> readInfraredCodeValue();     // 적외선 센서 (리모컨)

    // 키 데이터
    std::pair<bool, unsigned char> readKeyData();

private:
    I2CController& i2c;
};

#endif // RASPBOTSERVICE_H