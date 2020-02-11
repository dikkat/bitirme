#pragma once
#include <mysql.h>
#include <iostream>
#include "GeneralOperations.h"

namespace dbop {
	MYSQL* connectToDatabase(std::string server, std::string user, std::string password, std::string database);
	void disconnectFromDatabase(MYSQL* connect);
	std::string serializeMat(cv::Mat operand);
	cv::Mat deserializeMat(std::string operand);
}

