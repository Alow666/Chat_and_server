#include <iostream>

#include "TCP_Socket.h"
#include "SQL_queries.h"

#include <stdexcept> 
#include <string>  
#include <vector> 
#include <thread>
#include <libpq-fe.h>
#include <conio.h> 

#define PORT 50500
#define ADDRESS_IP4 "127.0.0.1"
#define BUFFER_SIZE 104857600

#define SQL_SERVER_IP "localHost"
#define SQL_PORT "5432"
#define SQL_SERVER_DB_NAME "chat"
#define SQL_SERVER_USER_NAME "postgres"
#define SQL_SERVER_PASSWORD "123"

void handleClient(const SOCKET& clientSocket) { //РАБОТА С СОКЕТОМ 

	PGconn* conn = PQsetdbLogin(SQL_SERVER_IP, SQL_PORT, NULL, NULL, SQL_SERVER_DB_NAME, SQL_SERVER_USER_NAME, SQL_SERVER_PASSWORD);

	if (PQstatus(conn) != CONNECTION_OK) {
		std::cerr << "ERROR connect DB: " << PQerrorMessage(conn) << std::endl;
		PQfinish(conn);
		return;
	}

	PQsetClientEncoding(conn, "UTF8");

	std::vector<char> buffer(BUFFER_SIZE);
	int bytesRead;// Количество переданных байт
	unsigned overSizePack = 0;//Количество всех байт 
	std::string data;
	bool sizeDataFlag = false;
	
	std::cout << "Handling client: " << clientSocket << std::endl;

	do {//Выйдет из цикла при возврате revc 0, а так же закрытии connect или ошибке

		bytesRead = ::recv(clientSocket, buffer.data(), BUFFER_SIZE, 0);

		if (bytesRead == SOCKET_ERROR) {//Если recv выбросил ошибку
			std::cerr << "Error receiving data from client (" << clientSocket << "): " << WSAGetLastError() << std::endl;
			break;
		}

		if (bytesRead == 0) {//Если пользователь отключился 
			std::cout << "Client (" << clientSocket << ") disconnected." << std::endl;
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

		if (data.length() == overSizePack) { //Проверка полного получения данных

			SQL_queries queries(conn);//Класс для работы с запросами SQL (DDL и DML)

			switch (data[0])
			{
			case '1':
				queries.DML_querie(data);
				break;

			case '2':

				if (queries.DDL_querie(data)) {
					data.clear();
					data = "true";
					break;
				}
				std::cout << "Not DDL query: " << clientSocket << " sock" << std::endl;
				data.clear();
				data = "false";
				break;

			default:

				data.clear();
				data = "Specify the query indicator!";
				break;
			}

			TCP_Socket::createPacketWithTextSize(data);
			::send(clientSocket, data.c_str(), int(data.length()), 0);//отправление результата SQL запроса
			overSizePack = 0;
			sizeDataFlag = false;
			data.clear();
		}
		buffer.clear();

	} while (bytesRead > 0);

	PQfinish(conn);
	closesocket(clientSocket);
}

int main() {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "WSAStartup failed." << std::endl;
		return 1;
	}

	int activity;
	const char exit = 27; //ESC
	std::atomic<bool> exitflag = true;

	try {
		TCP_Socket sock;
		sock.bind(ADDRESS_IP4, PORT);
		sock.listen(10);
		
		std::cout << "Server: " << ADDRESS_IP4 << ":" << PORT << " OPEN!" << std::endl;
		timeval timeout{ 1, 0 };

		std::cout << "To close the server press 'ESC'" << std::endl;
		std::thread exitThread(
			[&exitflag, exit]() {
				char x;
				do{
					x = _getch();
					if (exit == x) {
						exitflag = false;
					}
				}while(exitflag);
			});

		while (exitflag) {

			fd_set read_fds;
			FD_ZERO(&read_fds);             // Очищаем набор
			FD_SET(sock.getSocket(), &read_fds); // Добавляем слушающий сокет

			activity = select(NULL, &read_fds, NULL, NULL, &timeout);

			if (activity < 0) {
				int error_code = WSAGetLastError();
				if (error_code == WSAEINTR) continue;
			}

			if (activity == 0) {
				std::cout << "No new connections. Doing background work..." << std::endl;
				continue;
			}

			SOCKET clientSocket = sock.accept();
			std::cout << "Client connected: " << clientSocket << std::endl;
			std::thread clientThread(handleClient, std::cref(clientSocket));//ПОТОМ СДЕЛАТЬ ПУЛ ПОТОКОВ
			clientThread.detach();
		}
		sock.close();
		exitThread.join();
	}
	
	catch (const std::runtime_error& other) {
		std::cerr << "ERROR: " << other.what();
	}
	
	WSACleanup();
	return 0;
}