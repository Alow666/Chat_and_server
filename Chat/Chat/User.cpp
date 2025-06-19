#include "User.h"

User::User(std::string login, std::string pass, std::string name, std::string surname) :
	_login(login),
	_name(name),
	_surname(surname) {
	char* c_str = const_cast<char*>(pass.c_str());
	unsigned int* temp = sha1(c_str, sizeof(c_str));
	_pass.assign(temp, temp+5);
	delete[] temp;
};

User::~User() {
};

bool User::pass_check(std::string pass) {
	char* c_str = const_cast<char*>(pass.c_str());
	unsigned int* temp = sha1(c_str, sizeof(c_str));
	for (int i = 0; i < 5; i++) {
		if (temp[i] != _pass[i]) {
			delete[] temp;
			return false;
		};
	}
	delete[] temp;
	return true;
	
};

//void User::add_friend(unsigned int number) {//Добавить в друзья
//
//};
//
//void User::del_friend(unsigned int number) {//Удалить из друзей
//
//};