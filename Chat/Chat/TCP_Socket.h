#pragma once
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdexcept>
#include <string>
#include <vector>

#define BUFFER_SIZE 1024
#define PORT 50500

#pragma comment (lib, "Ws2_32.lib")

class TCP_Socket
{
public:
    TCP_Socket();
    ~TCP_Socket();

    bool connect(const std::string& address);

    void send(const std::string& data);

    std::string& recv();

    void close();

 
private:
    
    SOCKET sock;
    sockaddr_in serverAddr;
    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(BUFFER_SIZE);
    int bytesRead;
    std::vector<char> receivedDataBuffer;
    std::string text;
    const std::string END_MARKER = ";"; // маркер
};

