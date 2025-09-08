#include "Raspbot.h"
#include <iostream>

Raspbot::Raspbot() : dev(1, 0x2B) {} // I2C-1, addr 0x2B

void Raspbot::setMotor(int id, int dir, int speed) {
    unsigned char buf[4] = {0x01, (unsigned char)id, (unsigned char)dir, (unsigned char)speed};
    dev.writeData(buf, 4);
}

void Raspbot::setServo(int id, int angle) {
    unsigned char buf[3] = {0x02, (unsigned char)id, (unsigned char)angle};
    dev.writeData(buf, 3);
}

void Raspbot::buzzer(bool on) {
    unsigned char buf[2] = {0x06, (unsigned char)(on ? 1 : 0)};
    dev.writeData(buf, 2);
}

int Raspbot::readUltrasonic() {
    unsigned char low, high;
    dev.readData(0x1A, &low, 1);
    dev.readData(0x1B, &high, 1);
    return ((high << 8) | low);
}

unsigned char Raspbot::readLineSensor() {
    unsigned char val;
    dev.readData(0x0A, &val, 1);
    return val;
}