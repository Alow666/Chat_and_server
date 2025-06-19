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

	//void add_friend(unsigned int number);//�������� � ������
	//void del_friend(unsigned int number);//������� �� ������

	void send_message(unsigned int number, std::string text);//��������� ���������
	void my_message();//��� ���������

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
		std::string sender;//�� ����  
		std::string text;// ����� ���������
	};

	//std::map<std::string, std::string> friends_list;//������ ������
	//std::map<std::string, std::string> user_list; //������ �������������




};