#pragma once
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <iomanip> // Äëÿ std::setw, std::setfill
#include <sstream> // Äëÿ std::stringstream
#include <algorithm> // Äëÿ std::copy

#define BUFFER_SIZE 104857600
#define PORT 50500

#pragma comment (lib, "Ws2_32.lib")

class TCP_Socket
{
public:
    TCP_Socket();
    ~TCP_Socket();

    bool connect(const std::string& address);
    void send(std::string& data);
    std::string recv();
    void close();
  
private:
    
    SOCKET sock;
    sockaddr_in serverAddr;
    
    void createPacketWithTextSize(std::string& data);
    int extractSizeFromPacketWithTextSize(const std::vector<char>& data);

};

