#include "SQL_queries.h"

void SQL_queries::DML_querie(std::string& data)
{
	PGresult* res = PQexec(conn, data.c_str() + 1);//Сюда потом вставить запросы

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
			data += ' ';
		}
	}

	PQclear(res);
};

bool SQL_queries::DDL_querie(std::string& data) {//"INSERT INTO students (name, age) VALUES ($1, $2)|Alice, 20"

	std::string command; //Команда
	std::vector<std::string> arguments;//Аргументы
	std::vector<int> length_arguments;//Аргументы
	std::vector<int> param_formats_arguments;//Аргументы
	int arguments_count = 0;
	std::vector<const char*> param_values_c_array;

	char query_params_delimiter = '|';

	size_t delimiter_pos = data.find(query_params_delimiter);//Позиция разделителя

	if (delimiter_pos == std::string::npos) {
		std::cerr << "ERROR: Not delimiter '|'" << std::endl;
	}

	command = data.substr(1, delimiter_pos - 1);
	std::string arg = data.substr(delimiter_pos + 1);
	std::string temp;

	arg.erase(std::remove_if(arg.begin(), arg.end(), ::isspace), arg.end());//Отчистка от пробелов

	std::stringstream ss(arg);
	query_params_delimiter = ',';

	while (std::getline(ss, temp, query_params_delimiter)) {//Перенос всех аргументов в вектор 
		arguments.push_back(temp.c_str());
	}

	for (auto& x : arguments) {//Заполнения отальных векторов 
		length_arguments.push_back(static_cast<int>(x.length()));
		param_formats_arguments.push_back(0);
		arguments_count++;
	}

	param_values_c_array.reserve(arguments_count);

	for (const auto& x : arguments) {
		param_values_c_array.push_back(x.c_str());
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