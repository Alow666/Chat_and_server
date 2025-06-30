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

void TCP_Socket::recv(std::string& data) {
    std::vector<char> buffer(BUFFER_SIZE);
    int bytesRead = 0;
    unsigned overSizePack = 0;//���������� ���� ���� 
    bool sizeDataFlag = false;
    
    while((bytesRead = ::recv(sock, buffer.data(), BUFFER_SIZE, 0)) > 0) {//������ �� ����� ��� �������� revc 0, � ��� �� �������� connect ��� ������

        if (bytesRead == SOCKET_ERROR) {//���� recv �������� ������
            std::cerr << "Error receiving data!" << std::endl;
        }
        if (bytesRead == 0) {
            std::cerr << "Error server slep!" << std::endl;
        }
        if (sizeDataFlag == false && bytesRead > 10) {
            overSizePack = TCP_Socket::extractSizeFromPacketWithTextSize(buffer);
            sizeDataFlag = true;
        }

        data.append(buffer.begin(), buffer.begin() + bytesRead);

        //if (data.length() == overSizePack) { //�������� ������� ��������� ������
        //    break;
        //}
    }
}
    

void TCP_Socket::close() {
    if (sock != INVALID_SOCKET) {
        closesocket(sock);
        sock = INVALID_SOCKET;
    }
}

void TCP_Socket::createPacketWithTextSize(std::string& data) {//���������� � ������ ������� ������������ ������
    const int HEADER_SIZE_BYTES = 10;//������ ��������� int
    unsigned payload_length = static_cast<unsigned>(data.length());//������ ������
    std::stringstream ss;

    ss << std::setw(HEADER_SIZE_BYTES) /*������������� ������*/ << std::setfill('0') /*��������� �� 0*/ << payload_length; //����������� ������ �� ��������� � ����� 

    std::string size_header_str = ss.str();

    if (static_cast<int>(size_header_str.length()) > HEADER_SIZE_BYTES) {
        std::cerr << "ERROR: Too much data to send!" << std::endl;
        return;
    }
    size_header_str += data;
    data = size_header_str;
}

int TCP_Socket::extractSizeFromPacketWithTextSize(const std::vector<char>& data) { //���������� ������� ������ 

    const int HEADER_SIZE_BYTES = 10;
    int extracted_size = 0;
    std::string size(data.begin(), data.begin() + HEADER_SIZE_BYTES); // ��������� ��������� ������������� �������

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

