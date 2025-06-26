#pragma once
#include <libpq-fe.h>
#include <string>  
#include <vector> 
#include <iostream>
#include <sstream> // Äëÿ std::stringstream

class SQL_queries
{
public:
	SQL_queries(PGconn* x): conn(x){};
	~SQL_queries() {};

	void DML_querie(std::string& data);

	bool DDL_querie(std::string& data);

private:
	PGconn* conn;
};


