#pragma once
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdexcept>
#include <string>
#include <iomanip> // Äëÿ std::setw, std::setfill
#include <sstream> // Äëÿ std::stringstream
#include <vector>

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
    static void createPacketWithTextSize(std::string& data);
    static int extractSizeFromPacketWithTextSize(const std::vector<char>& data);

private:
    SOCKET sock;
};

