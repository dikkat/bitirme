#pragma once
#include <mysql.h>
#include <iostream>
#include "feat.h"
#include "image.h"
#include <sqlite3.h>
#include <xxhash.h>

namespace dbop {
	class Database {
	public:
		Database(std::string dbName);
		void insert_Image(std::string dir);
		void insert_Image(img::Image img);
		void insert_Histogram(int flag, int fb, int sb, int tb);
		void insert_Histogram(feat::Histogram hist);
		void insert_Edge(int flag, feat::Edge::Canny* edcOper = nullptr);
		void insert_Edge(feat::Edge edge);
		void insert_Icon(cv::Mat iconMat_src);
		void insert_Icon(img::Icon icon);
		void insert_Corner(XXH64_hash_t cornerHash, int flag, int numberOfScales, float scaleRatio, XXH64_hash_t cdhHash);
		void insert_CornerHarris(XXH64_hash_t cdhHash, int radius, int squareSize, float sigmai, float sigmad, float alpha,
			cv::Mat kernelx, cv::Mat kernely);
		void insert_ImageHistogram(XXH64_hash_t imHash, XXH64_hash_t histHash);
		void insert_ImageEdge(XXH64_hash_t imHash, XXH64_hash_t edgeHash);
		void insert_ImageCorner(XXH64_hash_t imHash, XXH64_hash_t cornerHash);
		void insert_ImageIcon(XXH64_hash_t imHash, XXH64_hash_t iconHash);
		std::vector<std::vector<std::string>> select_GENERAL(std::vector<std::vector<std::string>> paramVec);
		std::vector<std::string> select_Hash(std::string hashAbb, std::string className);
	private:
		void initializeDatabase(std::string dbName);
		void errorCheck(int rc, char* zErrMsg);
		static int callback(void* NotUsed, int argc, char** argv, char** azColName);
		static int callbackSELECT(void* data, int argc, char** argv, char** azColName);
		void initializeTables();
		sqlite3* db;
		char* zErrMsg = 0;
	};

	std::string serializeMat(cv::Mat operand);
	cv::Mat deserializeMat(std::string operand);
}