#pragma once
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdexcept>
#include <string>

#pragma comment (lib, "Ws2_32.lib")

class TCP_Socket
{
public:
    TCP_Socket();
    ~TCP_Socket();

    void bind(const std::string& ipAddress, int port);
    void listen(int backlog);
    SOCKET accept();
    int send(const std::string& data);
    int recv(char* buffer, int len);
    void close();
    SOCKET getHandle() const;
private:
    SOCKET sock;
};

