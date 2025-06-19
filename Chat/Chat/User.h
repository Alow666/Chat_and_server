#pragma once
#include <string>
#include <map>
#include <vector>
#include <memory>
#include "SHA1.h"


class User {
public:
	
	User() = delete;
	User(std::string login, std::string pass, std::string name, std::string surname);

	bool pass_check(std::string pass);

	//void add_friend(unsigned int number);//Добавить в друзья
	//void del_friend(unsigned int number);//Удалить из друзей

	void send_message(unsigned int number, std::string text);//Отправить сообщение
	void my_message();//мои сообщения

	/*void new_chat();
	void del_chat();*/

	~User();

private:

	std::string _login;
	std::vector <unsigned int> _pass;
	std::string _name;
	std::string _surname;

	struct Message
	{
		std::string sender;//от кого  
		std::string text;// текст сообщения
	};

	//std::map<std::string, std::string> friends_list;//Список друзей
	//std::map<std::string, std::string> user_list; //Список пользователей




};