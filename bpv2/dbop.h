#pragma once
#include <iostream>
#include "iop.h"
#include <sqlite3.h>
#include <xxhash.h>

class MainWindow;

namespace dbop {
	class Database {
	public:
		Database(const string& dbName);
		void insert_Image(const string& dir);
		void insert_Image(const img::Image& image);
		void insert_SourceImage(const img::Image& image);
		void insert_DestinationImage(const img::Image& image);
		void insert_Similarity(const iop::Comparison& comp);
		void insert_WeightVector(const iop::WeightVector& wvec);

		/*[0]ATTRIBUTES [1]TABLES [2]CONDITIONS*/
		std::vector<std::vector<string>> select_GENERAL(std::vector<std::vector<string>> paramVec);
		std::vector<string> select_Hash(const string& hashAbb, const string& className, const string& condition = "");
		img::Image Database::select_SourceImage();
		img::Image Database::select_DestinationImage();
		std::vector<std::vector<string>> select_Similarity();

		void delete_GENERAL(const string& table, const string& condition = "");

		sqlite3* databasePtr();
	private:
		friend class MainWindow;
		void initializeDatabase(const string& dbName);
		void errorCheck(int rc, char* zErrMsg);
		void initializeTables();
		sqlite3* db;
		char* zErrMsg = 0;
	};
}