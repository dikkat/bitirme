#pragma once
#include <mysql.h>
#include <iostream>
#include "gen.h"
#include <sqlite3.h>

namespace dbop {
	class Database {
	public:
		Database(std::string dbName);
		void insert_Image(XXH64_hash_t imHash, std::string name, std::string dir);
		void insert_Histogram(XXH64_hash_t histHash, int fb, int sb, int tb, int flag);
		void insert_Edge(XXH64_hash_t edgeHash, int flag, XXH64_hash_t edcHash);
		void insert_Corner(XXH64_hash_t cornerHash, int flag, int numberOfScales, float scaleRatio, XXH64_hash_t cdhHash);
		void insert_CornerHarris(XXH64_hash_t cdhHash, int radius, int squareSize, float sigmai, float sigmad, float alpha,
			cv::Mat kernelx, cv::Mat kernely);
		void insert_ImageHistogram(XXH64_hash_t imHash, XXH64_hash_t histHash);
		void insert_ImageEdge(XXH64_hash_t imHash, XXH64_hash_t edgeHash);
		void insert_ImageCorner(XXH64_hash_t imHash, XXH64_hash_t cornerHash);
	private:
		void initializeDatabase(std::string dbName);
		void errorCheck(int rc, char* zErrMsg);
		static int callback(void* NotUsed, int argc, char** argv, char** azColName);
		static int callbackSELECT(void* data, int argc, char** argv, char** azColName);
		void initializeTables();
		sqlite3* dbPtr;
		char* sql;
		char* zErrMsg = 0;
	};

	extern Database dbObj;

	std::string serializeMat(cv::Mat operand);
	cv::Mat deserializeMat(std::string operand);
}