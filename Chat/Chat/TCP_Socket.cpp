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

void TCP_Socket::send(std::string& data) {

    createPacketWithTextSize(data);
    
    if (!(::send(sock, data.c_str(), static_cast<int>(data.size()), 0)))
        std::cout << "Not send!" << std::endl;
}

std::string TCP_Socket::recv(){
    std::vector<char> buffer(BUFFER_SIZE);
    int bytesRead = 0;
    unsigned overSizePack = 0;//Количество всех байт 
    bool sizeDataFlag = false;
    std::string data;

    do{

        bytesRead = ::recv(sock, buffer.data(), BUFFER_SIZE, 0);

        if (bytesRead == SOCKET_ERROR) {//Если recv выбросил ошибку
            std::cerr << "ERROR: server connection error > " << WSAGetLastError() << std::endl;
            break;
        }

        if (bytesRead == 0) {//Если пользователь отключился 
            std::cout << "ERROR: The server is sleeping" << std::endl;
            break;
        }

        if (sizeDataFlag == false && bytesRead > 10) {
            overSizePack = TCP_Socket::extractSizeFromPacketWithTextSize(buffer);
            data.append(buffer.begin() + 10, buffer.begin() + bytesRead);
            sizeDataFlag = true;
        }
        else {
            data.append(buffer.begin(), buffer.begin() + bytesRead);
        }

        if (data.length() == overSizePack) {
            return data;
        }

    } while (bytesRead > 0);
}
    

void TCP_Socket::close() {
    if (sock != INVALID_SOCKET) {
        closesocket(sock);
        sock = INVALID_SOCKET;
    }
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

int TCP_Socket::extractSizeFromPacketWithTextSize(const std::vector<char>& data) { //Извлечение размера данных 

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

