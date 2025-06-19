#include "TCP_Socket.h"


TCP_Socket::TCP_Socket() : sock(INVALID_SOCKET) {
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        throw std::runtime_error("Failed to create socket");
    }
}

TCP_Socket::~TCP_Socket() {
    if (sock != INVALID_SOCKET) {
        closesocket(sock);
    }
}

void TCP_Socket::bind(const std::string& ipAddress, int port) {
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, ipAddress.c_str(), &addr.sin_addr);
    addr.sin_port = htons(port);
    if (::bind(sock, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        throw std::runtime_error("Failed to bind socket");
    }
}

void TCP_Socket::listen(int backlog) {
    if (::listen(sock, backlog) == SOCKET_ERROR) {
        throw std::runtime_error("Failed to listen on socket");
    }
}

SOCKET TCP_Socket::accept() {
    SOCKET clientSock = ::accept(sock, nullptr, nullptr);
    if (clientSock == INVALID_SOCKET) {
        throw std::runtime_error("Failed to accept connection");
    }
    return clientSock;
}

int TCP_Socket::send(const std::string& data) {
    return ::send(sock, data.c_str(), static_cast<int>(data.length()), 0);
}

int TCP_Socket::recv(char* buffer, int len) {
    return ::recv(sock, buffer, len, 0);
}

void TCP_Socket::close() {
    if (sock != INVALID_SOCKET) {
        closesocket(sock);
        sock = INVALID_SOCKET;
    }
}

SOCKET TCP_Socket::getHandle() const {
    return sock;
}