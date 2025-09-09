#include "TcpServer.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <thread>

TcpServer::TcpServer(int port, MessageHandler handler)
    : port(port), messageHandler(handler), server_fd(-1), addrlen(sizeof(address)), running(false) {
    
    // 소켓 생성
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "오류: 소켓 생성 실패." << std::endl;
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 소켓 옵션 설정 (빠른 재시작)
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::cerr << "오류: 소켓 옵션 설정 실패." << std::endl;
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 주소 구조체 설정
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;   // 모든 인터페이스에서 수신
    address.sin_port = htons(this->port);   // 호스트 바이트 순서를 네트워크 바이트 순서로 변환

    // 소켓에 주소 바인딩
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "오류: 소켓 바인딩 실패." << std::endl;
        close(server_fd);
        exit(EXIT_FAILURE);
    }
}

TcpServer::~TcpServer() {
    stop();
    if (server_fd != -1) {
        close(server_fd);
    }
    std::cout << "TcpServer 소멸자 호출: 서버 종료." << std::endl;
}

bool TcpServer::startListening() {
    // 클라이언트 연결 요청 대기 시작
    if (listen(server_fd, 3) < 0) {
        std::cerr << "오류: 리스닝 실패" << std::endl;
        return false;
    }
    std::cout << "TcpServer:포트: " << port << "에서 리스닝 시작." << std::endl;
    running = true;
    return true;
}

void TcpServer::run() {
    if (!running) {
        std::cerr << "오류: 서버가 시작되기 전에 run() 호출됨." << std::endl;
        return;
    }
    while(running) {
        int client_socket;
        client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (client_socket < 0) {
            if (running) { // stop() 호출로 인해 accept가 중단된 경우
                std::cerr << "오류: 클라이언트 연결 수락 실패." << std::endl;
            }
            continue;
        }

        std::cout << "클라이언트 연결됨: " << client_socket << std::endl;
        // 각 클라이언트를 별도의 스레드에서 처리
        std::thread clientThread(&TcpServer::handleClient, this, client_socket);
        clientThread.detach(); // 스레드 분리
    }
}

void TcpServer::stop() {
    running = false;
    if (server_fd != -1) {
        // listen 중인 소켓을 닫아 accep 중단
        shutdown(server_fd, SHUT_RDWR); // 강제 소켓 종료
        close(server_fd);
        server_fd = -1;
        std::cout << "TcpServer: 서버 중지됨." << std::endl;
    }
}

void TcpServer::handleClient(int client_socket) {
    char buffer[1024] = {0};
    int valread;

    while ((valread = read(client_socket, buffer, 1024)) > 0) {
        buffer[valread] = '\0'; // 문자열 종료 문자 추가
        std::string request(buffer);
        std::cout << "수신된 요청 (클라이언트 " << client_socket << "): " << request << std::endl;

        // 콜백 함수 호출하여 응답 생성
        std::string response = messageHandler(request);

        // 응답 전송
        send(client_socket, response.c_str(), response.length(), 0);
        std::cout << "전송된 응답 (클라이언트 " << client_socket << "): " << response << std::endl;
        memset(buffer, 0, sizeof(buffer)); // 버퍼 초기화
    }

    if (valread == 0) {
        std::cout << "클라이언트 " << client_socket << " 연결 종료." << std::endl;
    } else { // valread < 0
        std::cerr << "오류: 클라이언트 " << client_socket << " 읽기 오류 또는 연결 끊김." << std::endl;
    }

    close(client_socket); // 클라이언트 소켓 닫기
}