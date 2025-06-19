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

#define SQL_SERVER_IP "localHost"
#define SQL_PORT "5432"
#define SQL_SERVER_DB_NAME "chat"
#define SQL_SERVER_USER_NAME "postgres"
#define SQL_SERVER_PASSWORD "123"

void handleClient(const SOCKET& clientSocket) { //������ � ������� 

	PGconn* conn = conn = PQsetdbLogin(SQL_SERVER_IP, SQL_PORT, NULL, NULL, SQL_SERVER_DB_NAME, SQL_SERVER_USER_NAME, SQL_SERVER_PASSWORD);

	if (PQstatus(conn) != CONNECTION_OK) {
		std::cerr << "ERROR connect DB: " << PQerrorMessage(conn) << std::endl;
		PQfinish(conn);
		return;
	}

	PQsetClientEncoding(conn, "UTF8");

	std::unique_ptr<char[]> buffer = std::make_unique<char[]>(BUFFER_SIZE);
	int bytesRead;
	std::vector<char> receivedDataBuffer;
	std::string SQL_Send;
	const std::string END_MARKER = ";"; // ������
	const char* temp;
	
	std::cout << "Handling client: " << clientSocket << std::endl;

	while ((bytesRead = ::recv(clientSocket, buffer.get(), BUFFER_SIZE, 0)) > 0) {//������ �� ����� ��� �������� connect ��� ������
		
		receivedDataBuffer.insert(receivedDataBuffer.end(), buffer.get(), buffer.get() + bytesRead); //������� � ������ ���� ���� � ������
		
		if (!receivedDataBuffer.empty() && receivedDataBuffer.back() == END_MARKER[0]) { //���� ������ ����� ������ �����
			
			// ������ ����� ���������� ���������� ������ � ������������ SQL-������
			
			std::string receivedData(receivedDataBuffer.begin(), receivedDataBuffer.end()); 

			PGresult* res = PQexec(conn, receivedData.c_str());//���� ����� �������� �������

			if (PQresultStatus(res) != PGRES_TUPLES_OK) {
				std::cout << "ERROR DB: " << PQerrorMessage(conn) << std::endl;
				PQclear(res);
				PQfinish(conn);
			}

			int rows = PQntuples(res);
			int cols = PQnfields(res);

			for (int i = 0; i < rows; i++) {
				for (int j = 0; j < cols; j++) {
					temp = PQgetvalue(res, i, j);
					SQL_Send += temp;
					SQL_Send += " ";
				}
				
			}

			SQL_Send += ";";

			PQclear(res);

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

	PQfinish(conn);
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

			std::thread clientThread(handleClient, std::ref(clientSocket));
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