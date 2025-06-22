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

void handleClient(const SOCKET& clientSocket) { //РАБОТА С СОКЕТОМ 

	PGconn* conn = conn = PQsetdbLogin(SQL_SERVER_IP, SQL_PORT, NULL, NULL, SQL_SERVER_DB_NAME, SQL_SERVER_USER_NAME, SQL_SERVER_PASSWORD);

	if (PQstatus(conn) != CONNECTION_OK) {
		std::cerr << "ERROR connect DB: " << PQerrorMessage(conn) << std::endl;
		PQfinish(conn);
		return;
	}

	PQsetClientEncoding(conn, "UTF8");

	std::unique_ptr<char[]> buffer = std::make_unique<char[]>(BUFFER_SIZE);
	int bytesRead;
	const std::string END_MARKER = ";"; // маркер
	const char* temp;
	std::string data;
	
	std::cout << "Handling client: " << clientSocket << std::endl;

	while ((bytesRead = ::recv(clientSocket, buffer.get(), BUFFER_SIZE, 0)) > 0) {//Выйдет из цикла при закрытии connect или ошибке

		data.append(buffer.get(), buffer.get() + bytesRead);

		if (!data.empty() && data.back() == END_MARKER[0]) { //Если вектор имеет маркер конца
			
			// Теперь можно обработать полученные данные и сформировать SQL-запрос
			
			PGresult* res = PQexec(conn, data.c_str());//Сюда потом вставить запросы

			if (PQresultStatus(res) != PGRES_TUPLES_OK) {
				std::cout << "ERROR DB: " << PQerrorMessage(conn) << std::endl;
				PQclear(res);
				PQfinish(conn);
			}

			data.clear();
			int rows = PQntuples(res);
			int cols = PQnfields(res);

			for (int i = 0; i < rows; i++) {
				for (int j = 0; j < cols; j++) {
					temp = PQgetvalue(res, i, j);
					data += temp;
					data += " ";
				}
			}

			data += ";";

			PQclear(res);

			::send(clientSocket, data.c_str(), data.length(), 0);//отправление результата SQL запроса
			
			data.clear();
		}
	}

	if (bytesRead == SOCKET_ERROR) {//Если recv выбросил ошибку
		std::cerr << "Error receiving data from client (" << clientSocket << "): " << WSAGetLastError() << std::endl;
	}
	else if (bytesRead == 0) {//Если пользователь отключился 
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

		std::cout << "Server: "<< ADDRESS_IP4 <<":"<<PORT<< " OPEN!" << std::endl;//РАЗОБРАТЬСЯ

		while (true) {

			SOCKET clientSocket = sock.accept();
			
			std::cout << "Client connected: " << clientSocket << std::endl;//РАЗОБРАТЬСЯ

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