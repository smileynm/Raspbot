#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <string>
#include <functional>
#include <sys/socket.h>
#include <netinet/in.h>

class TcpServer {
public:
    // 클라이언트로부터 메시지 받을 경우 호출될 콜백 함수 타입 정의
    using MessageHandler = std::function<std::string(const std::string&)>;

    TcpServer(int port, MessageHandler handler);
    ~TcpServer();

    bool startListening();
    void run();
    void stop();

private:
    int server_fd;
    int port;
    struct sockaddr_in address;
    int addrlen;
    MessageHandler messageHandler;
    volatile bool running;       // 서버 실행 상태 플래그

    void handleClient(int client_socket);
};

#endif // TCPSERVER_H