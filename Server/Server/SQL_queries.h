#pragma once
#include <libpq-fe.h>
#include <string>  
#include <vector> 
#include <iostream>

class SQL_queries
{
public:
	SQL_queries(PGconn* x): conn(x){};
	~SQL_queries() {};

	void DML_querie(std::string& data)
	{
		PGresult* res = PQexec(conn, data.c_str()+1);//—юда потом вставить запросы

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
				if ((i == rows - 1) && (i == rows - 1)) data += ";";
			}
		}

		PQclear(res);
	};

	bool DDL_querie(const std::string& data){
	
	};

private:
	PGconn* conn;
};