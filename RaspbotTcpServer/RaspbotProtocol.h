#ifndef RASPBOTPROTOCOL_H
#define RASPBOTPROTOCOL_H

#define RASPBOT_I2C_ADDRESS 0x2B

namespace RaspbotReg {
    // 쓰기 전용 레지스터 목록
    const unsigned char MOTOR_SPEED                 = 0x01;         // 모터 속도 제어
    const unsigned char SERVO                       = 0x02;         // 서보 제어
    const unsigned char RGB_ALL_FIXED_BRIGHTNESS    = 0x03;         // RGB LED 전체 고정 밝기
    const unsigned char RGB_INDIVIDUAL_FIXED_BRIGHTNESS = 0x04;     // RGB LED 개별 고정 밝기
    const unsigned char IR_REMOTE_CONTROL_SWITCH    = 0x05;         // 적외선 리모컨 제어 스위치
    const unsigned char BUZZER                      = 0x06;         // 부저 제어
    const unsigned char ULTRASONIC_CONTROL_SWITCH   = 0x07;         // 초음파 센서 제어 스위치
    const unsigned char RGB_ALL_BRIGHTNESS          = 0x08;         // RGB LED 전체 밝기
    const unsigned char RGB_INDIVIDUAL_BRIGHTNESS   = 0x09;         // RGB LED 개별 밝기

    // Read Registers
    const unsigned char IR_SENSOR_DATA              = 0x0A;         // 적외선 센서 데이터
    const unsigned char IR_CODE_VALUE_DATA          = 0x0C;         // 적외선 코드 값 데이터
    const unsigned char KEY_DATA                    = 0x0D;         // 버튼 데이터
    const unsigned char ULTRASONIC_DATA_LOW_BIT     = 0x1A;         // 초음파 센서 데이터 (Low Byte)
    const unsigned char ULTRASONIC_DATA_HIGH_BIT    = 0x1B;         // 초음파 센서 데이터 (High Byte)
}

// 고정 밝기 모드 정의 (0x03, 0x04 레지스터용)
enum class RgbColor {
    RED = 0x0,
    GREEN,
    BLUE,
    YELLOW,
    PURPLE,
    INDIGO,
    WHITE,
    OFF // 이 값은 buf[1]=0일 때 (off) 유효
};

// 모터 번호
enum class MotorNumber {
    L1 = 0x00,
    L2 = 0x01,
    R1 = 0x02,
    R2 = 0x03
};

// 모터 방향
enum class MotorDirection {
    FORWARD = 0x00,
    BACKWARD = 0x01
};

// 전체 전원 제어
enum class DeviceStatus {
    OFF = 0x00,
    ON = 0x01
};

#endif // RASPBOTPROTOCOL_H