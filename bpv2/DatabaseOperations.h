#pragma once
#include <mysql.h>
#include <iostream>
#include "GeneralOperations.h"
#include <sqlite3.h>




namespace dbop {
	MYSQL* connectToDatabaseMYSQL(std::string server, std::string user, std::string password, std::string database);
	void disconnectFromDatabaseMYSQL(MYSQL* connect);
	bool connectToDatabaseSQLITE(const char* file);
	std::string serializeMat(cv::Mat operand);
	cv::Mat deserializeMat(std::string operand);
}

