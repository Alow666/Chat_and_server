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

bool TCP_Socket::connect(const std::string& address) {
   
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, address.c_str(), &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid address: " << address << std::endl;
        return false;
    }

    if (::connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connect failed with error: " << WSAGetLastError() << std::endl;
        return false;
    }
    return true;
}

void TCP_Socket::send(const std::string& data) {
    
    if (!(::send(sock, data.c_str(), static_cast<int>(data.length()), 0)))
        std::cout << "Not send!" << std::endl;

}

std::string& TCP_Socket::recv() {

    text.clear();
    receivedDataBuffer.clear();

    while ((bytesRead = ::recv(sock, buffer.get(), BUFFER_SIZE, 0)) > 0) {//Выйдет из цикла при закрытии connect или ошибке

        receivedDataBuffer.insert(receivedDataBuffer.end(), buffer.get(), buffer.get() + bytesRead); //Перенос в вектор всех байт с буфера

        if (!receivedDataBuffer.empty() && receivedDataBuffer.back() == END_MARKER[0]) { //Если вектор имеет маркер конца
            std::cout << "End marker received." << std::endl;
            

            text.insert(text.end(), receivedDataBuffer.begin(), receivedDataBuffer.end());

            receivedDataBuffer.clear(); // Очищаем буфер для следующих данных
            break;
        }
    }

    if (bytesRead > 0) {//Если recv выбросил ошибку
        std::cerr << "Error receiving data!"<< std::endl;
    }
    
    return text;

}
    

void TCP_Socket::close() {
    if (sock != INVALID_SOCKET) {
        closesocket(sock);
        sock = INVALID_SOCKET;
    }
}

