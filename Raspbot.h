#pragma once
#include "I2CDevice.h"

class Raspbot {
public:
    Raspbot();
    void setMotor(int id, int dir, int speed);
    void setServo(int id, int angle);
    void buzzer(bool on);
    int readUltrasonic();
    unsigned char readLineSensor();

private:
    I2CDevice dev;
};