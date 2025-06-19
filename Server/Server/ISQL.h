#pragma once
#include <libpq-fe.h>
#include <iostream>

#define SQL_SERVER_IP "localHost"
#define SQL_PORT "5432"
#define SQL_SERVER_DB_NAME "chat"
#define SQL_SERVER_USER_NAME "postgres"
#define SQL_SERVER_PASSWORD "123"


class SQL_interface
{
public:
	SQL_interface() { 
		conn = PQsetdbLogin(SQL_SERVER_IP, SQL_PORT, NULL, NULL, SQL_SERVER_DB_NAME, SQL_SERVER_USER_NAME, SQL_SERVER_PASSWORD);

		if (PQstatus(conn) != CONNECTION_OK) {
			std::cerr << "Ошибка подключения к базе данных: " << PQerrorMessage(conn) << std::endl;
			PQfinish(conn);
		}
	}

	void sending_request(std::string textRequest){

	}

	~SQL_interface() {
		PQfinish(conn);
		PQclear(res);
	}



private:
	PGconn* conn;
	PGresult* res;
};
