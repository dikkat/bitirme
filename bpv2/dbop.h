#pragma once
#include <mysql.h>
#include <iostream>
#include "gen.h"
#include <sqlite3.h>

namespace dbop {
	void initializeDatabase();
	void errorCheck(int rc, char* zErrMsg);
	void initializeTables(sqlite3* db);
	static int callback(void* NotUsed, int argc, char** argv, char** azColName);
	std::string serializeMat(cv::Mat operand);
	cv::Mat deserializeMat(std::string operand);
}