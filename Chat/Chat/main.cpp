#include <iostream>
#include "TCP_Socket.h"
#include <stdexcept> 
#include <string>  
#include <vector> 
#include <Windows.h>
//"2INSERT INTO users (name, second_name, login, number_phone, email, hash_pass) VALUES ($1, $2, $3, $4, $5, $6)|Alice, Wunder, Alow_, +79039471795, merda@mail.ru, 123"
#define SERVER_ADDRESS "127.0.0.1" // Локальный адрес сервера

int main() {
	SetConsoleOutputCP(CP_UTF8);

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "WSAStartup failed." << std::endl;
		return 1;
	}

	std::string aa("1select name, second_name from users where id = 2");
	
	TCP_Socket sock;
	sock.connect(SERVER_ADDRESS);
		
	sock.send(aa);
	std::string aaa;
	aaa = sock.recv();

	std::cout << aaa << std::endl;

	
	sock.close();
	WSACleanup();
	return 0;
	
}