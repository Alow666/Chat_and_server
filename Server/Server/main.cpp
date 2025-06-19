#include <iostream>
#include "TCP_Socket.h"
#include <stdexcept> 
#include <string>  
#include <vector> 
#include <thread>
#include <libpq-fe.h>


#define PORT 50500
#define ADDRESS_IP4 "127.0.0.1"
#define BUFFER_SIZE 1024

void handleClient(SOCKET clientSocket) { //������ � ������� ����������
	std::unique_ptr<char[]> buffer = std::make_unique<char[]>(BUFFER_SIZE);
	int bytesRead;
	std::vector<char> receivedDataBuffer;
	std::string SQL_Send;
	const std::string END_MARKER = ";"; // ������
	
	std::cout << "Handling client: " << clientSocket << std::endl;
	

	while ((bytesRead = ::recv(clientSocket, buffer.get(), BUFFER_SIZE, 0)) > 0) {//������ �� ����� ��� �������� connect ��� ������
		
		receivedDataBuffer.insert(receivedDataBuffer.end(), buffer.get(), buffer.get() + bytesRead); //������� � ������ ���� ���� � ������
		
		if (!receivedDataBuffer.empty() && receivedDataBuffer.back() == END_MARKER[0]) { //���� ������ ����� ������ �����
			std::cout << "End marker received." << std::endl;
			// ������ ����� ���������� ���������� ������ � ������������ SQL-������


			std::string receivedData(receivedDataBuffer.begin(), receivedDataBuffer.end()); 

			SQL_Send = "SELECT * FROM your_table WHERE data = 123123123123;"; 



			::send(clientSocket, SQL_Send.c_str(), SQL_Send.length(), 0);//����������� ���������� SQL �������
			receivedDataBuffer.clear(); // ������� ����� ��� ��������� ������
		}
	}

	if (bytesRead == SOCKET_ERROR) {//���� recv �������� ������
		std::cerr << "Error receiving data from client (" << clientSocket << "): " << WSAGetLastError() << std::endl;
	}
	else if (bytesRead == 0) {//���� ������������ ���������� 
		std::cout << "Client (" << clientSocket << ") disconnected." << std::endl;
	}

	closesocket(clientSocket);
}

int main() {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "WSAStartup failed." << std::endl;
		return 1;
	}


	try {
		TCP_Socket sock;
		sock.bind(ADDRESS_IP4, PORT);
		sock.listen(10);

		std::cout << "Server: "<< ADDRESS_IP4 <<":"<<PORT<< " OPEN!" << std::endl;//�����������

		while (true) {

			SOCKET clientSocket = sock.accept();
			
			std::cout << "Client connected: " << clientSocket << std::endl;//�����������

			std::thread clientThread(handleClient, clientSocket);
			clientThread.detach();
		}
		sock.close();
	}
	
	catch (const std::runtime_error& other) {
		std::cerr << "ERROR: " << other.what();
	}

	WSACleanup();
	return 0;
}