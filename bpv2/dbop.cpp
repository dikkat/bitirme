#include "dbop.h"

dbop::Database::Database(const string& dbName) {
	initializeDatabase(dbName);
}

void dbop::Database::initializeDatabase(const string& dbName) {
	sqlite3* dbLocal;
	int rc;

	rc = sqlite3_open(dbName.c_str(), &dbLocal);

	if (rc) {
		throw std::exception(("Can't open database: %s\n", sqlite3_errmsg(dbLocal)));
	}

	db = dbLocal;

	initializeTables();
}

void dbop::Database::errorCheck(int rc, char* zErrMsg) {
	if (rc != SQLITE_OK && rc != SQLITE_DONE) {
		string errMsg = zErrMsg;
		if (errMsg.find("UNIQUE") != string::npos);
		else
			throw std::exception(("SQL error: %s\n", zErrMsg));
	}
}

void dbop::Database::initializeTables() {
	char* sql = 0;
	char** resultp;
	int row, col, rc;
	sqlite3_stmt* strQuery = NULL;

	sql = "SELECT name FROM sqlite_master WHERE type='table' and name = 'Image';";
	rc = sqlite3_get_table(db, sql, &resultp, &row, &col, &zErrMsg);
	errorCheck(rc, zErrMsg);
	if (row == 0) {
		sql = "CREATE TABLE Image(" \
			"hash TEXT PRIMARY KEY NOT NULL," \
			"name TEXT NOT NULL," \
			"dir TEXT NOT NULL);";
		strQuery = NULL;
		rc = sqlite3_prepare_v2(db, sql, strlen(sql), &strQuery, NULL);
		errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
		rc = sqlite3_step(strQuery);
		errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
	}

	sql = "SELECT name FROM sqlite_master WHERE type='table' and name = 'Similarity';";
	rc = sqlite3_get_table(db, sql, &resultp, &row, &col, &zErrMsg);
	errorCheck(rc, zErrMsg);
	if (row == 0) {
		sql = "CREATE TABLE Similarity(" \
			"srcHash TEXT NOT NULL," \
			"destHash TEXT NOT NULL," \
			"srcDir TEXT NOT NULL," \
			"destDir TEXT NOT NULL," \
			"diff_gradm REAL," \
			"diff_gradd REAL," \
			"diff_hgray REAL," \
			"diff_hbgrb REAL," \
			"diff_hbgrg REAL," \
			"diff_hbgrr REAL," \
			"diff_hhsvh REAL," \
			"diff_hhsvs REAL," \
			"diff_hhsvv REAL," \
			"diff_hashd REAL," \
			"diff_hashp REAL," \
			"similarity REAL," \
			"FOREIGN KEY (srcHash)" \
			"REFERENCES Image(hash)" \
			"ON DELETE CASCADE," \
			"FOREIGN KEY (destHash)" \
			"REFERENCES Image(hash)" \
			"ON DELETE CASCADE," \
			"PRIMARY KEY (srcHash, destHash));";
		strQuery = NULL;
		rc = sqlite3_prepare_v2(db, sql, strlen(sql), &strQuery, NULL);
		errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
		rc = sqlite3_step(strQuery);
		errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
	}

	sql = "SELECT name FROM sqlite_master WHERE type='table' and name = 'SourceImage';";
	rc = sqlite3_get_table(db, sql, &resultp, &row, &col, &zErrMsg);
	errorCheck(rc, zErrMsg);
	if (row == 0) {
		sql = "CREATE TABLE SourceImage(" \
			"hash TEXT PRIMARY KEY NOT NULL," \
			"name TEXT NOT NULL," \
			"dir TEXT NOT NULL);";
		strQuery = NULL;
		rc = sqlite3_prepare_v2(db, sql, strlen(sql), &strQuery, NULL);
		errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
		rc = sqlite3_step(strQuery);
		errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
	}

	sql = "SELECT name FROM sqlite_master WHERE type='table' and name = 'DestImage';";
	rc = sqlite3_get_table(db, sql, &resultp, &row, &col, &zErrMsg);
	errorCheck(rc, zErrMsg);
	if (row == 0) {
		sql = "CREATE TABLE DestImage(" \
			"hash TEXT PRIMARY KEY NOT NULL," \
			"name TEXT NOT NULL," \
			"dir TEXT NOT NULL);";
		strQuery = NULL;
		rc = sqlite3_prepare_v2(db, sql, strlen(sql), &strQuery, NULL);
		errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
		rc = sqlite3_step(strQuery);
		errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
	}

	sql = "SELECT name FROM sqlite_master WHERE type='table' and name = 'WeightVector';";
	rc = sqlite3_get_table(db, sql, &resultp, &row, &col, &zErrMsg);
	errorCheck(rc, zErrMsg);
	if (row == 0) {
		sql = "CREATE TABLE WeightVector(" \
			"w_gradm REAL," \
			"w_gradd REAL," \
			"w_hgray REAL," \
			"w_hbgrb REAL," \
			"w_hbgrg REAL," \
			"w_hbgrr REAL," \
			"w_hhsvh REAL," \
			"w_hhsvs REAL," \
			"w_hhsvv REAL," \
			"w_hashd REAL," \
			"w_hashp REAL);";
		strQuery = NULL;
		rc = sqlite3_prepare_v2(db, sql, strlen(sql), &strQuery, NULL);
		errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
		rc = sqlite3_step(strQuery);
		errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
	}
}

void dbop::Database::insert_Image(const string& dir) {
	img::Image imgOper(dir, cv::IMREAD_COLOR);
	string hash_str = std::to_string(imgOper.getHash());
	string name_str = imgOper.getImageName();

	sqlite3_stmt* strQuery = NULL;
	int rc = sqlite3_prepare_v2(db, "INSERT INTO image(hash, name, dir) VALUES (?, ?, ?)", -1, &strQuery, NULL);
	errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
	sqlite3_bind_text(strQuery, 1, hash_str.c_str(), hash_str.size(), SQLITE_STATIC);
	sqlite3_bind_text(strQuery, 2, name_str.c_str(), name_str.size(), SQLITE_STATIC);
	sqlite3_bind_text(strQuery, 3, dir.c_str(), dir.size(), SQLITE_STATIC);
	rc = sqlite3_step(strQuery);
	errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
}

void dbop::Database::insert_Image(const img::Image& image) {
	string hash_str = std::to_string(image.getHash());
	string name_str = image.getVariablesString()[0];
	string dir_str = image.getVariablesString()[1];

	sqlite3_stmt* strQuery = NULL;
	int rc = sqlite3_prepare_v2(db, "INSERT INTO image(hash, name, dir) VALUES (?, ?, ?)", -1, &strQuery, NULL);
	errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
	sqlite3_bind_text(strQuery, 1, hash_str.c_str(), hash_str.size(), SQLITE_STATIC);
	sqlite3_bind_text(strQuery, 2, name_str.c_str(), name_str.size(), SQLITE_STATIC);
	sqlite3_bind_text(strQuery, 3, dir_str.c_str(), dir_str.size(), SQLITE_STATIC);
	rc = sqlite3_step(strQuery);
	errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
}

void dbop::Database::insert_SourceImage(const img::Image& image) {
	string hash_str = std::to_string(image.getHash());
	string name_str = image.getVariablesString()[0];
	string dir_str = image.getVariablesString()[1];

	sqlite3_stmt* strQuery = NULL;
	int rc = sqlite3_prepare_v2(db, "INSERT INTO SourceImage(hash, name, dir) VALUES (?, ?, ?)", -1, &strQuery, NULL);
	errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
	sqlite3_bind_text(strQuery, 1, hash_str.c_str(), hash_str.size(), SQLITE_STATIC);
	sqlite3_bind_text(strQuery, 2, name_str.c_str(), name_str.size(), SQLITE_STATIC);
	sqlite3_bind_text(strQuery, 3, dir_str.c_str(), dir_str.size(), SQLITE_STATIC);
	rc = sqlite3_step(strQuery);
	errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
}

void dbop::Database::insert_Similarity(const iop::Comparison& comp) {
	img::Image lhand(comp.source_dir, cv::IMREAD_COLOR);
	img::Image rhand(comp.rhand_dir, cv::IMREAD_COLOR);
	string srchash_str = std::to_string(lhand.getHash());
	string desthash_str = std::to_string(rhand.getHash());
	string srcdir = lhand.getVariablesString()[1];
	string destdir = rhand.getVariablesString()[1];
	double diff_gradm = static_cast<double>(comp.diff_gradm);
	double diff_gradd = static_cast<double>(comp.diff_gradd);
	double diff_hgray = static_cast<double>(comp.diff_hgray);
	double diff_hbgrb = static_cast<double>(comp.diff_hbgrb);
	double diff_hbgrg = static_cast<double>(comp.diff_hbgrg);
	double diff_hbgrr = static_cast<double>(comp.diff_hbgrr);
	double diff_hhsvh = static_cast<double>(comp.diff_hhsvh);
	double diff_hhsvs = static_cast<double>(comp.diff_hhsvs);
	double diff_hhsvv = static_cast<double>(comp.diff_hhsvv);
	double diff_hashd = static_cast<double>(comp.diff_hashd);
	double diff_hashp = static_cast<double>(comp.diff_hashp);
	double similarity = static_cast<double>(comp.euc_dist);

	sqlite3_stmt* strQuery = NULL;
	int rc = sqlite3_prepare_v2(db, "INSERT INTO Similarity(srchash, desthash, srcdir, destdir," \
		"diff_gradm, diff_gradd, diff_hgray, diff_hbgrb, diff_hbgrg, diff_hbgrr, diff_hhsvh," \
		"diff_hhsvs, diff_hhsvv, diff_hashd, diff_hashp, similarity)" \
		"VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", -1, &strQuery, NULL);
	errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
	sqlite3_bind_text(strQuery, 1, srchash_str.c_str(), srchash_str.size(), SQLITE_STATIC);
	sqlite3_bind_text(strQuery, 2, desthash_str.c_str(), desthash_str.size(), SQLITE_STATIC);
	sqlite3_bind_text(strQuery, 3, srcdir.c_str(), srcdir.size(), SQLITE_STATIC);
	sqlite3_bind_text(strQuery, 4, destdir.c_str(), destdir.size(), SQLITE_STATIC);
	sqlite3_bind_double(strQuery, 5, diff_gradm);
	sqlite3_bind_double(strQuery, 6, diff_gradd);
	sqlite3_bind_double(strQuery, 7, diff_hgray);
	sqlite3_bind_double(strQuery, 8, diff_hbgrb);
	sqlite3_bind_double(strQuery, 9, diff_hbgrg);
	sqlite3_bind_double(strQuery, 10, diff_hbgrr);
	sqlite3_bind_double(strQuery, 11, diff_hhsvh);
	sqlite3_bind_double(strQuery, 12, diff_hhsvs);
	sqlite3_bind_double(strQuery, 13, diff_hhsvv);
	sqlite3_bind_double(strQuery, 14, diff_hashd);
	sqlite3_bind_double(strQuery, 15, diff_hashp);
	sqlite3_bind_double(strQuery, 16, similarity);
	rc = sqlite3_step(strQuery);
	errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
}

void dbop::Database::insert_DestinationImage(const img::Image& image) {
	string hash_str = std::to_string(image.getHash());
	string name_str = image.getVariablesString()[0];
	string dir_str = image.getVariablesString()[1];

	sqlite3_stmt* strQuery = NULL;
	int rc = sqlite3_prepare_v2(db, "INSERT INTO DestImage(hash, name, dir) VALUES (?, ?, ?)", -1, &strQuery, NULL);
	errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
	sqlite3_bind_text(strQuery, 1, hash_str.c_str(), hash_str.size(), SQLITE_STATIC);
	sqlite3_bind_text(strQuery, 2, name_str.c_str(), name_str.size(), SQLITE_STATIC);
	sqlite3_bind_text(strQuery, 3, dir_str.c_str(), dir_str.size(), SQLITE_STATIC);
	rc = sqlite3_step(strQuery);
	errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
}

void dbop::Database::insert_WeightVector(const iop::WeightVector& wvec) {
	double w_gradm = static_cast<double>(wvec.wv_grad[0]);
	double w_gradd = static_cast<double>(wvec.wv_grad[1]);
	double w_hgray = static_cast<double>(wvec.w_hgray);
	double w_hbgrb = static_cast<double>(wvec.wv_hbgr[0]);
	double w_hbgrg = static_cast<double>(wvec.wv_hbgr[1]);
	double w_hbgrr = static_cast<double>(wvec.wv_hbgr[2]);
	double w_hhsvh = static_cast<double>(wvec.wv_hhsv[0]);
	double w_hhsvs = static_cast<double>(wvec.wv_hhsv[1]);
	double w_hhsvv = static_cast<double>(wvec.wv_hhsv[2]);
	double w_hashd = static_cast<double>(wvec.wv_hash[0]);
	double w_hashp = static_cast<double>(wvec.wv_hash[1]);

	sqlite3_stmt* strQuery = NULL;
	int rc = sqlite3_prepare_v2(db, "INSERT INTO WeightVector(w_gradm, w_gradd, w_hgray," \
		"w_hbgrb, w_hbgrg, w_hbgrr, w_hhsvh, w_hhsvs, w_hhsvv, w_hashd, w_hashp) " \
		"VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", -1, &strQuery, NULL);
	errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
	sqlite3_bind_double(strQuery, 1, w_gradm);
	sqlite3_bind_double(strQuery, 2, w_gradd);
	sqlite3_bind_double(strQuery, 3, w_hgray);
	sqlite3_bind_double(strQuery, 4, w_hbgrb);
	sqlite3_bind_double(strQuery, 5, w_hbgrg);
	sqlite3_bind_double(strQuery, 6, w_hbgrr);
	sqlite3_bind_double(strQuery, 7, w_hhsvh);
	sqlite3_bind_double(strQuery, 8, w_hhsvs);
	sqlite3_bind_double(strQuery, 9, w_hhsvv);
	sqlite3_bind_double(strQuery, 10, w_hashd);
	sqlite3_bind_double(strQuery, 11, w_hashp);
	rc = sqlite3_step(strQuery);
	errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
}

std::vector<std::vector<string>> dbop::Database::select_GENERAL(std::vector<std::vector<string>> paramVec) {
	sqlite3_stmt* strQuery = NULL;

	string attributes = "";
	int att = 0;
	for (const string& i : paramVec[0]) {
		attributes.append(i);
		attributes.append(",");
		att++;
	}
	attributes.erase(attributes.end() - 1);

	string tables = "";
	for (const string& i : paramVec[1]) {
		tables.append(i);
		tables.append(",");
	}
	tables.erase(tables.end() - 1);

	string conditions = "";
	if (!paramVec[2].empty())
		conditions = paramVec[2][0];

	string statement;
	if (conditions != "")
		statement = "SELECT DISTINCT " + attributes + " FROM " + tables + " WHERE " + conditions + ";";
	else
		statement = "SELECT DISTINCT " + attributes + " FROM " + tables + ";";
	int rc = sqlite3_prepare_v2(db, statement.c_str(), -1, &strQuery, NULL);
	errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));

	std::vector<std::vector<string>> selectVec;
	int row = 0;
	for (int i = 0; i < att; i++)
		selectVec.push_back(std::vector<string>{});

	while (true) {
		switch (int rc = sqlite3_step(strQuery)) {
		case SQLITE_ROW:
			for (int i = 0; i < att; i++) {
				if (sqlite3_column_bytes(strQuery, 0 + i) == 0);
				else
					selectVec[i].push_back(reinterpret_cast<const char*>(sqlite3_column_text(strQuery, 0 + i)));
			}
			row++;
			break;
		case SQLITE_DONE:
			return selectVec;
		default:
			try {
				errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
			}
			catch (const std::exception& e) {
				std::cerr << e.what();
				return std::vector<std::vector<string>>{};
			}
		}
	}
	return std::vector<std::vector<string>>{};
}

std::vector<string> dbop::Database::select_Hash(const string& hashAbb, const string& className, const string& condition) {
	sqlite3_stmt* strQuery = NULL;

	int rc;
	string stmt;
	if (condition != "") {
		stmt = "SELECT " + hashAbb + "hash" + " FROM " + className + " WHERE " + condition + ";";
		rc = sqlite3_prepare_v2(db, stmt.c_str(), -1, &strQuery, NULL);
		errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
	}
	else {
		stmt = "SELECT " + hashAbb + "hash" + " FROM " + className;
		rc = sqlite3_prepare_v2(db, stmt.c_str(), -1, &strQuery, NULL);
		errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
	}

	std::vector<string> hashVec;
	int row = 0;
	bool done = false;
	while (!done) {
		switch (int rc = sqlite3_step(strQuery)) {
		case SQLITE_ROW:
			hashVec.push_back(reinterpret_cast<const char*>(sqlite3_column_text(strQuery, 0)));
			row++;
			break;
		case SQLITE_DONE:
			done = true;
			break;
		default:
			errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
			return std::vector<string>{"-1"};
		}
	}
	return hashVec;
}

img::Image dbop::Database::select_SourceImage() {
	sqlite3_stmt* strQuery = NULL;

	int rc = sqlite3_prepare_v2(db, "SELECT dir FROM SourceImage", -1, &strQuery, NULL);
	errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));

	std::vector<string> imgVec;
	int row = 0;
	bool done = false;
	const char* charOper;
	while (!done) {
		switch (int rc = sqlite3_step(strQuery)) {
		case SQLITE_ROW:
			charOper = reinterpret_cast<const char*>(sqlite3_column_text(strQuery, 0));
			imgVec.push_back(charOper);
			row++;
			break;
		case SQLITE_DONE:
			done = true;
			break;
		default:
			errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
		}
	}
	if (row > 1) {
		delete_GENERAL("SourceImage");
		throw std::exception("Multiple source images, somehow. Pretty much impossible error. Source image table cleared, load source image again even if it's still there.");
	}
	else if (row == 0) {
		throw std::exception("Unable to locate source image. Load image to source first.");
	}
	else {
		return img::Image(imgVec[0], cv::IMREAD_COLOR);
	}

}

img::Image dbop::Database::select_DestinationImage() {
	sqlite3_stmt* strQuery = NULL;

	int rc = sqlite3_prepare_v2(db, "SELECT dir FROM DestImage", -1, &strQuery, NULL);
	errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));

	std::vector<string> imgVec;
	int row = 0;
	bool done = false;
	const char* charOper;
	while (!done) {
		switch (int rc = sqlite3_step(strQuery)) {
		case SQLITE_ROW:
			charOper = reinterpret_cast<const char*>(sqlite3_column_text(strQuery, 0));
			imgVec.push_back(charOper);
			row++;
			break;
		case SQLITE_DONE:
			done = true;
			break;
		default:
			errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
		}
	}
	if (row > 1) {
		delete_GENERAL("SourceImage");
		throw std::exception("Multiple right hand images, somehow. Pretty much impossible error. Right hand image table cleared, load right hand image again even if it's still there.");
	}
	else if (row == 0) {
		throw std::exception("Unable to locate right hand image. Load image to right hand first.");
	}
	else {
		return img::Image(imgVec[0], cv::IMREAD_COLOR);
	}
}

std::vector<std::vector<string>> dbop::Database::select_Similarity() {
	sqlite3_stmt* strQuery = NULL;

	int rc = sqlite3_prepare_v2(db, "SELECT DISTINCT b.dir, similarity.similarity" \
		" FROM image a, image b" \
		" INNER JOIN similarity" \
		" ON similarity.srchash = a.hash and similarity.desthash = b.hash", -1, &strQuery, NULL);
	errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));

	std::vector<std::vector<string>> selectVec;
	int row = 0;
	int att = 2;
	for (int i = 0; i < att; i++)
		selectVec.push_back(std::vector<string>{});

	while (true) {
		switch (int rc = sqlite3_step(strQuery)) {
		case SQLITE_ROW:
			for (int i = 0; i < att; i++) {
				if (sqlite3_column_bytes(strQuery, 0 + i) == 0);
				else
					selectVec[i].push_back(reinterpret_cast<const char*>(sqlite3_column_text(strQuery, 0 + i)));
			}
			row++;
			break;
		case SQLITE_DONE:
			return selectVec;
		default:
			try {
				errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
			}
			catch (const std::exception& e) {
				std::cerr << e.what();
				return std::vector<std::vector<string>>{};
			}
		}
	}
	return std::vector<std::vector<string>>{};
}

void dbop::Database::delete_GENERAL(const string& table, const string& conditions) {
	sqlite3_stmt* strQuery = NULL;

	string statement;
	if (conditions != "")
		statement = "DELETE FROM " + table + " WHERE " + conditions + ";";
	else
		statement = "DELETE FROM " + table + ";";
	int rc = sqlite3_prepare_v2(db, statement.c_str(), -1, &strQuery, NULL);
	errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
	rc = sqlite3_step(strQuery);
	errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
}

sqlite3* dbop::Database::databasePtr() {
	return this->db;
}