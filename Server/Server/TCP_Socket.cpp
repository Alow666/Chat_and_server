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

SOCKET& TCP_Socket::getSocket() {
    return sock;
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
    u_long mode = 1;
    if (ioctlsocket(sock, FIONBIO, &mode) != 0) { 
        throw std::runtime_error("Failed not block socket");
    }
}

SOCKET TCP_Socket::accept() {
    SOCKET clientSock = ::accept(sock, nullptr, nullptr);
    if (clientSock == INVALID_SOCKET) {
        throw std::runtime_error("Failed to accept connection");
    }
    return clientSock;
}

int TCP_Socket::send(std::string& data) {
    createPacketWithTextSize(data);
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

void TCP_Socket::createPacketWithTextSize(std::string& data) {//Добавление в голову размера передаваемых данных

    const int HEADER_SIZE_BYTES = 10;//Размер заголовка int
    unsigned payload_length = static_cast<unsigned>(data.length());//Размер данных
    std::stringstream ss;

    ss << std::setw(HEADER_SIZE_BYTES) /*Устанавливаем ширину*/ << std::setfill('0') /*Заполняем ее 0*/ << payload_length; //Вставляется размер по умолчанию с права 

    std::string size_header_str = ss.str();

    if (static_cast<int>(size_header_str.length()) > HEADER_SIZE_BYTES) {
        std::cerr << "ERROR: Too much data to send!" << std::endl;
        return;
    }
    size_header_str += data;
    data = size_header_str;
}

int TCP_Socket::extractSizeFromPacketWithTextSize(const std::vector<char>& data){ //Извлечение размера данных 

    const int HEADER_SIZE_BYTES = 10;
    int extracted_size = 0;
    std::string size(data.begin(), data.begin() + HEADER_SIZE_BYTES); // Извлекаем строковое представление размера
    
    try {
        extracted_size = std::stoi(size);
    }
    catch (const std::invalid_argument& e) {
        std::cerr << "ERROR: The size header contains non-numeric characters: " << e.what() << std::endl;
    }
    catch (const std::out_of_range& e) {
        std::cerr << "ERROR: Header size out of int range: " << e.what() << std::endl;
    }

    return extracted_size;
}