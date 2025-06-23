#pragma once
#include <libpq-fe.h>
#include <string>  
#include <vector> 
#include <iostream>
#include <iomanip> // Для std::setw, std::setfill
#include <sstream> // Для std::stringstream
#include <memory>

class SQL_queries
{
public:
	SQL_queries(PGconn* x): conn(x){};
	~SQL_queries() {};

	void DML_querie(std::string& data)
	{


		PGresult* res = PQexec(conn, data.c_str()+1);//Сюда потом вставить запросы

		if (PQresultStatus(res) != PGRES_TUPLES_OK) {
			std::cerr << "ERROR DB: " << PQerrorMessage(conn) << std::endl;
			PQclear(res);
			PQfinish(conn);
		}

		data.clear();
		int rows = PQntuples(res);
		int cols = PQnfields(res);

		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				data += PQgetvalue(res, i, j);
			}
		}

		PQclear(res);
	};

	bool DDL_querie(std::string& data){

		std::string command; //Команда
		std::vector<std::string> arguments;//Аргументы
		std::vector<int> length_arguments;//Аргументы
		std::vector<int> param_formats_arguments;//Аргументы
		int arguments_count = 0;
		std::vector<const char*> param_values_c_array;

		char query_params_delimiter = '|';

		size_t delimiter_pos = data.find(query_params_delimiter);// Находим позицию разделителя

		if (delimiter_pos == std::string::npos) {
			std::cerr << "Ошибка: Разделитель '|' между запросом и параметрами не найден." << std::endl;
		}

		command = data.substr(0, delimiter_pos);
		std::string arg = data.substr(delimiter_pos + 1);
		std::string temp;

		arg.erase(std::remove_if(arg.begin(), arg.end(), ::isspace), arg.end());

		std::stringstream ss(arg);
		query_params_delimiter = ',';

		while (std::getline(ss, temp, query_params_delimiter)) {
			arguments.push_back(temp.c_str());
		}

		for (auto& x : arguments) {
			length_arguments.push_back(static_cast<int>(x.length()));
			param_formats_arguments.push_back(0);
			arguments_count++;
		}

		param_values_c_array.reserve(arguments_count);

		for (size_t i = 0; i < arguments_count; ++i) {
			         param_values_c_array[i] = arguments[i].c_str();
			     }

		PGresult* res = PQexecParams(
			conn,
			command.c_str(),
			static_cast<int>(arguments.size()),
			NULL,
			param_values_c_array.data(),
			length_arguments.data(),
			param_formats_arguments.data(),
			0
		);

		if (PQresultStatus(res) != PGRES_COMMAND_OK) {
			PQclear(res);
			return false;
		}

		PQclear(res);
		return true;
	};

private:
	PGconn* conn;
};


