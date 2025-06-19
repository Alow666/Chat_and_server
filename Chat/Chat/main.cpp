#include <iostream>
#include "TCP_Socket.h"
#include <stdexcept> 
#include <string>  
#include <vector> 

#define SERVER_ADDRESS "127.0.0.1" // Локальный адрес сервера

int main() {

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "WSAStartup failed." << std::endl;
		return 1;
	}

	std::string aa("ASDASDASdD;");
	
	TCP_Socket sock;
	sock.connect(SERVER_ADDRESS);
		
	sock.send(aa);

	aa = sock.recv();

	std::cout << aa << std::endl;

	
	sock.close();
	WSACleanup();
	return 0;
	
}