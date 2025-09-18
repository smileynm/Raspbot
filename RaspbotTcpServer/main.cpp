#include <iostream>
#include <string>
#include <stdexcept>
#include <nlohmann/json.hpp>

#include "I2CController.h"
#include "RaspbotProtocol.h"
#include "RaspbotService.h"
#include "TcpServer.h"

// nlohmann::json을 더 짧게 사용하기 위한 별칭
using json = nlohmann::json;

// RESTful API 요청을 처리하는 함수 (TcpServer에 전달될 MessageHandler)
std::string handleRestApiRequest(RaspbotService& raspbotService, const std::string& request) {
    json response;
    try {
        // req_json은 client가 요청한 request를 json형태로 따온 것 (parse)
        json req_json = json::parse(request);

        // req_json 객체에서 "endpoint" 키를 찾음
        // 만약 endpoint를 찾지 못하면, 빈 문자열 ("")을 반환
        std::string endpoint = req_json.value("endpoint", "");

        if (endpoint.empty()) {
            response["status"] = "error";
            response["message"] = "엔드포인트가 지정되지 않음.";
            // json형태의 데이터를 string으로 변환하여 return.  (dump())
            return response.dump();
        }

        // --- 제어 로직 시작 ---
        bool success = false;
        std::string message = "";

        // 모터 제어
        if (endpoint == "/motor") {
            success = raspbotService.controlMotor(
                static_cast<MotorNumber>(req_json.value("motor_number", 0)),
                static_cast<MotorDirection>(req_json.value("direction", 0)),
                req_json.value("speed", 0));
            message = success ? "모터 제어 성공" : "모터 제어 실패";
        }
        
        // 서보 제어
        else if (endpoint == "/servo") {
            success = raspbotService.controlServo(
                req_json.value("servo_number", 0),
                req_json.value("angle", 0));
            message = success ? "서보 제어 성공" : "서보 제어 실패";
        }
        
        // 전체 RGB 제어
        else if (endpoint == "/rgb/all") {
            success = raspbotService.controlRgbAll(
                static_cast<DeviceStatus>(req_json.value("status", 0)),
                static_cast<RgbColor>(req_json.value("color", 0)));
            message = success ? "RGB 전체 제어 성공" : "RGB 전체 제어 실패";
        } 
        
        // 개별 RGB 제어
        else if (endpoint == "/rgb/individual") {
            success = raspbotService.controlRgbIndividual(
                req_json.value("led_number", 0),
                static_cast<DeviceStatus>(req_json.value("status", 0)),
                static_cast<RgbColor>(req_json.value("color", 0)));
            message = success ? "RGB 개별 제어 성공" : "RGB 개별 제어 실패";
        }
        
        // 전체 RGB 밝기 제어
        else if (endpoint == "/rgb/brightness/all") {
            success = raspbotService.setRgbAllBrightness(
                req_json.value("r", 0),
                req_json.value("g", 0),
                req_json.value("b", 0));
            message = success ? "RGB 전체 밝기 제어 성공" : "RGB 전체 밝기 제어 실패";
        }
        
        // 개별 RGB 밝기 제어
        else if (endpoint == "/rgb/brightness/individual") {
            success = raspbotService.setRgbIndividualBrightness(
                req_json.value("led_number", 0),
                req_json.value("r", 0),
                req_json.value("g", 0),
                req_json.value("b", 0));
            message = success ? "RGB 개별 밝기 제어 성공" : "RGB 개별 밝기 제어 실패";
        }
        
        // 부저 제어
        else if (endpoint == "/buzzer") {
            success = raspbotService.controlBuzzer(
                static_cast<DeviceStatus>(req_json.value("status", 0)));
            message = success ? "부저 제어 성공" : "부저 제어 실패";
        }
        
        // 초음파 센서 제어
        else if (endpoint == "/ultrasonic") {
            success = raspbotService.controlUltrasonic(
                static_cast<DeviceStatus>(req_json.value("status", 0)));
            message = success ? "초음파 센서 제어 성공" : "초음파 센서 제어 실패";
        }
        
        // 초음파 거리 읽기
        else if (endpoint == "/ultrasonic/read") {
            auto result = raspbotService.readUltrasonicDistance();

            // std::pair<bool, T> -> 첫 번째 값은 성공/실패 여부 (first), 두 번째는 센서 데이터 값
            if (result.first) { // 첫 번째 요소는 성공 여부
                response["status"] = "success";
                response["distance"] = result.second; // 두 번째 요소는 실제 값
                return response.dump();
            } else {
                success = false;
                message = "초음파 거리 읽기 실패";
            }
        }
        
        // 적외선 센서 읽기 (라인트레이싱)
        else if (endpoint == "/ir/sensor") {
            auto result = raspbotService.readInfraredSensorData();
            if (result.first) {
                response["status"] = "success";
                response["value"] = result.second;
                return response.dump();
            } else {
                success = false;
                message = "IR 센서 데이터 읽기 실패";
            }
        }
        
        // 적외선 센서 읽기 (리모컨 제어 코드)
        else if (endpoint == "/ir/code") {
            auto result = raspbotService.readInfraredCodeValue();
            if (result.first) {
                response["status"] = "success";
                response["value"] = result.second;
                return response.dump();
            } else {
                success = false;
                message = "IR 코드 데이터 읽기 실패";
            }
        }
        
        // Raspbot의 물리적 버튼 (0x00: 눌려있지 않은 상태, 0x01: 눌린 상태)
        else if (endpoint == "/key/read") {
            auto result = raspbotService.readKeyData();
            if (result.first) {
                response["status"] = "success";
                response["value"] = result.second;
                return response.dump();
            } else {
                success = false;
                message = "키 데이터 읽기 실패";
            }
        }
        
        // 명시되지 않은 엔드포인트가 request되었을 경우
        else {
            response["status"] = "error";
            response["message"] = "알 수 없는 엔드포인트: " + endpoint;
            return response.dump();
        }

        // --- 제어 로직 끝 ---

        response["status"] = success ? "success" : "error";
        response["message"] = message;

    } catch (const json::parse_error& e) {
        response["status"] = "error";
        response["message"] = "JSON 파싱 오류: " + std::string(e.what());
    } catch (const json::exception& e) {
        response["status"] = "error";
        response["message"] = "JSON 처리 오류: " + std::string(e.what());
    } catch (const std::exception& e) {
        response["status"] = "error";
        response["message"] = "일반 오류: " + std::string(e.what());
    } catch (...) {
        response["status"] = "error";
        response["message"] = "알 수 없는 오류 발생.";
    }

    return response.dump();
}

int main(int argc, char* argv[]) {
    // I2C 컨트롤러 초기화
    I2CController i2cController(RASPBOT_I2C_ADDRESS);
    if (!i2cController.openDevice()) {
        std::cerr << "I2C 장치 초기화 실패. 서버 종료." << std::endl;
        return EXIT_FAILURE;
    }

    // Raspbot 서비스 초기화
    RaspbotService raspbotService(i2cController);

    // 서버 포트 설정
    int serverPort = 8080;
    if (argc > 1) {
        try {
            serverPort = std::stoi(argv[1]);
        } catch (const std::exception& e) {
            std::cerr << "경고: 잘못된 포트 번호. 기본 포트 " << serverPort << "를 사용." << std::endl;
        }
    }

    // TCP 서버 초기화 및 요청 핸들러 설정
    // 람다 함수를 사용하여 raspbotService 객체를 handleRestApiRequest에 바인딩
    TcpServer server(serverPort, [&](const std::string& req){
        return handleRestApiRequest(raspbotService, req);
    });

    if (!server.startListening()) {
        std::cerr << "TCP 서버 시작 실패. 프로그램 종료." << std::endl;
        return EXIT_FAILURE;
    }

    // 서버 실행 (무한 루프)
    server.run();

    // 서버 종료 (Ctrl+C 등으로 종료될 경우 호출)
    i2cController.closeDevice();
    return EXIT_SUCCESS;
}