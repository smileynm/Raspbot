#include "Raspbot.h"
#include <unistd.h>
#include <iostream>

int main() {
    Raspbot bot;

    // bot.setMotor(0, 0, 200);
    // bot.setMotor(1, 0, 200);
    // bot.setMotor(2, 0, 200);
    // bot.setMotor(3, 0, 200);
    // sleep(2);
    // bot.setMotor(0, 0, 0);
    // bot.setMotor(1, 0, 0);
    // bot.setMotor(2, 0, 0);
    // bot.setMotor(3, 0, 0);

    bot.setServo(1, 90);
    bot.setServo(0, 90);
    sleep(1);

    //bot.buzzer(true);
    sleep(1);
    bot.buzzer(false);

    int dist = bot.readUltrasonic();
    std::cout << "Distance: " << dist << "mm\n";

    unsigned char line = bot.readLineSensor();
    std::cout << "Line sensor: " << (int)line << "\n";

    return 0;
}