#include "dbop.h"

dbop::Database::Database(string dbName) {
    initializeDatabase(dbName);
}

void dbop::Database::initializeDatabase(string dbName) {
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

    sql = "PRAGMA foreign_keys = ON;";

    sql = "SELECT name FROM sqlite_master WHERE type='table' and name = 'Image';";
    rc = sqlite3_get_table(db, sql, &resultp, &row, &col, &zErrMsg);
    errorCheck(rc,zErrMsg);
    if (row == 0) {
        sql = "CREATE TABLE Image(" \
            "hash TEXT PRIMARY KEY NOT NULL," \
            "name TEXT NOT NULL," \
            "dir TEXT NOT NULL);";
        rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
        errorCheck(rc, zErrMsg);
    }

    sql = "SELECT name FROM sqlite_master WHERE type='table' and name = 'Histogram';";
    rc = sqlite3_get_table(db, sql, &resultp, &row, &col, &zErrMsg);
    errorCheck(rc,zErrMsg);
    if (row == 0) {
        sql = "CREATE TABLE Histogram(" \
            "hash TEXT PRIMARY KEY NOT NULL," \
            "flag INT NOT NULL," \
            "fbin INT NOT NULL," \
            "sbin INT NOT NULL," \
            "tbin INT NOT NULL);";
        rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
        errorCheck(rc, zErrMsg);
    }

    sql = "SELECT name FROM sqlite_master WHERE type='table' and name = 'EdgeCanny';";
    rc = sqlite3_get_table(db, sql, &resultp, &row, &col, &zErrMsg);
    errorCheck(rc,zErrMsg);
    if (row == 0) {
        sql = "CREATE TABLE EdgeCanny(" \
            "hash TEXT PRIMARY KEY NOT NULL," \
            "gaussKernelSize INT NOT NULL," \
            "sigma REAL NOT NULL," \
            "thigh REAL NOT NULL," \
            "tlow REAL NOT NULL," \
            "kernelx BLOB NOT NULL," \
            "kernely BLOB NOT NULL);";
        rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
        errorCheck(rc, zErrMsg);
    }

    sql = "SELECT name FROM sqlite_master WHERE type='table' and name = 'Edge';";
    rc = sqlite3_get_table(db, sql, &resultp, &row, &col, &zErrMsg);
    errorCheck(rc,zErrMsg);
    if (row == 0) {
        sql = "CREATE TABLE Edge(" \
            "hash TEXT PRIMARY KEY NOT NULL," \
            "flag INT NOT NULL," \
            "edcHash TEXT," \
            "FOREIGN KEY (edcHash)" \
            "REFERENCES EdgeCanny(hash)" \
            "ON DELETE CASCADE);";
        rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
        errorCheck(rc, zErrMsg);
    }

    sql = "SELECT name FROM sqlite_master WHERE type='table' and name = 'CornerHarris';";
    rc = sqlite3_get_table(db, sql, &resultp, &row, &col, &zErrMsg);
    errorCheck(rc,zErrMsg);
    if (row == 0) {
        sql = "CREATE TABLE CornerHarris(" \
            "hash TEXT PRIMARY KEY NOT NULL," \
            "radius INT NOT NULL," \
            "squareSize INT NOT NULL," \
            "sigmai REAL NOT NULL," \
            "sigmad REAL NOT NULL," \
            "alpha REAL NOT NULL," \
            "kernelx BLOB NOT NULL," \
            "kernely BLOB NOT NULL);";
        rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
        errorCheck(rc, zErrMsg);
    }

    sql = "SELECT name FROM sqlite_master WHERE type='table' and name = 'Corner';";
    rc = sqlite3_get_table(db, sql, &resultp, &row, &col, &zErrMsg);
    errorCheck(rc,zErrMsg);
    if (row == 0) {
        sql = "CREATE TABLE Corner(" \
            "hash TEXT PRIMARY KEY NOT NULL," \
            "flag INT NOT NULL," \
            "cdhHash TEXT NOT NULL," \
            "numberOfScales INT NOT NULL," \
            "scaleRatio REAL," \
            "FOREIGN KEY (cdhHash)" \
            "REFERENCES EdgeCanny(hash)" \
            "ON DELETE CASCADE);";
        rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
        errorCheck(rc, zErrMsg);
    }

    sql = "SELECT name FROM sqlite_master WHERE type='table' and name = 'Icon';";
    rc = sqlite3_get_table(db, sql, &resultp, &row, &col, &zErrMsg);
    errorCheck(rc, zErrMsg);
    if (row == 0) {
        sql = "CREATE TABLE Icon(" \
            "hash TEXT PRIMARY KEY NOT NULL," \
            "mat BLOB NOT NULL);";
        rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
        errorCheck(rc, zErrMsg);
    }

    sql = "SELECT name FROM sqlite_master WHERE type='table' and name = 'ImageHistogram';";
    rc = sqlite3_get_table(db, sql, &resultp, &row, &col, &zErrMsg);
    errorCheck(rc,zErrMsg);
    if (row == 0) {
        sql = "CREATE TABLE ImageHistogram(" \
            "imHash TEXT NOT NULL," \
            "histHash TEXT NOT NULL," \
            "FOREIGN KEY (imHash)" \
            "REFERENCES Image(hash)" \
            "ON DELETE CASCADE," \
            "FOREIGN KEY (histHash)" \
            "REFERENCES Histogram(hash)" \
            "ON DELETE CASCADE," \
            "PRIMARY KEY (imHash, histHash));";
        rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
        errorCheck(rc, zErrMsg);
    }

    sql = "SELECT name FROM sqlite_master WHERE type='table' and name = 'ImageEdge';";
    rc = sqlite3_get_table(db, sql, &resultp, &row, &col, &zErrMsg);
    errorCheck(rc,zErrMsg);
    if (row == 0) {
        sql = "CREATE TABLE ImageEdge(" \
            "imHash TEXT NOT NULL," \
            "edgeHash TEXT NOT NULL," \
            "FOREIGN KEY (imHash)" \
            "REFERENCES Image(hash)" \
            "ON DELETE CASCADE," \
            "FOREIGN KEY (edgeHash)" \
            "REFERENCES Edge(hash)" \
            "ON DELETE CASCADE," \
            "PRIMARY KEY (imHash, edgeHash));";
        rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
        errorCheck(rc, zErrMsg);
    }    

    sql = "SELECT name FROM sqlite_master WHERE type='table' and name = 'ImageCorner';";
    rc = sqlite3_get_table(db, sql, &resultp, &row, &col, &zErrMsg);
    errorCheck(rc,zErrMsg);
    if (row == 0) {
        sql = "CREATE TABLE ImageCorner(" \
            "imHash TEXT NOT NULL," \
            "cornerHash TEXT NOT NULL," \
            "FOREIGN KEY (imHash)" \
            "REFERENCES Image(hash)" \
            "ON DELETE CASCADE," \
            "FOREIGN KEY (cornerHash)" \
            "REFERENCES Edge(hash)" \
            "ON DELETE CASCADE," \
            "PRIMARY KEY (imHash, cornerHash));";
        rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
        errorCheck(rc, zErrMsg);
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
        rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
        errorCheck(rc, zErrMsg);
    }

    sql = "SELECT name FROM sqlite_master WHERE type='table' and name = 'ImageIcon';";
    rc = sqlite3_get_table(db, sql, &resultp, &row, &col, &zErrMsg);
    errorCheck(rc, zErrMsg);
    if (row == 0) {
        sql = "CREATE TABLE ImageIcon(" \
            "imHash TEXT NOT NULL," \
            "iconHash TEXT NOT NULL," \
            "FOREIGN KEY (imHash)" \
            "REFERENCES Image(hash)" \
            "ON DELETE CASCADE," \
            "FOREIGN KEY (iconHash)" \
            "REFERENCES Icon(hash)" \
            "ON DELETE CASCADE," \
            "PRIMARY KEY (imHash, iconHash));";
        rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
        errorCheck(rc, zErrMsg);
    }

    sql = "SELECT name FROM sqlite_master WHERE type='table' and name = 'SourceImage';";
    rc = sqlite3_get_table(db, sql, &resultp, &row, &col, &zErrMsg);
    errorCheck(rc,zErrMsg);
    if (row == 0) {
        sql = "CREATE TABLE SourceImage(" \
            "hash TEXT PRIMARY KEY NOT NULL," \
            "name TEXT NOT NULL," \
            "dir TEXT NOT NULL);";
        rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
        errorCheck(rc, zErrMsg);
    }

    sql = "SELECT name FROM sqlite_master WHERE type='table' and name = 'DestImage';";
    rc = sqlite3_get_table(db, sql, &resultp, &row, &col, &zErrMsg);
    errorCheck(rc, zErrMsg);
    if (row == 0) {
        sql = "CREATE TABLE DestImage(" \
            "hash TEXT PRIMARY KEY NOT NULL," \
            "name TEXT NOT NULL," \
            "dir TEXT NOT NULL);";
        rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
        errorCheck(rc, zErrMsg);
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
		rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
		errorCheck(rc, zErrMsg);
	}
}

int dbop::Database::callback(void* data, int argc, char** argv, char** azColName) {
    int i;
    fprintf(stderr, "%s: ", (const char*)data);

    for (i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }

    printf("\n");
    return 0;
}

void dbop::Database::insert_Image(string dir) {
    img::Image imgOper(dir, cv::IMREAD_COLOR);
    string hash_str = std::to_string(imgOper.getHash());
    string name_str = imgOper.getImageName();

    sqlite3_stmt* strQuery = NULL;
    int rc = sqlite3_prepare_v2(db, "INSERT INTO image(hash, name, dir) VALUES (?, ?, ?)", -1, &strQuery, NULL);
    sqlite3_bind_text(strQuery, 1, hash_str.c_str(), hash_str.size(), SQLITE_STATIC);
    sqlite3_bind_text(strQuery, 2, name_str.c_str(), name_str.size(), SQLITE_STATIC);
    sqlite3_bind_text(strQuery, 3, dir.c_str(), dir.size(), SQLITE_STATIC);
    rc = sqlite3_step(strQuery);
    errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
}

void dbop::Database::insert_Image(img::Image image) {
    string hash_str = std::to_string(image.getHash());
    string name_str = image.getVariablesString()[0];
    string dir_str = image.getVariablesString()[1];
    
    sqlite3_stmt* strQuery = NULL;
    int rc = sqlite3_prepare_v2(db, "INSERT INTO image(hash, name, dir) VALUES (?, ?, ?)", -1, &strQuery, NULL);
    sqlite3_bind_text(strQuery, 1, hash_str.c_str(), hash_str.size(), SQLITE_STATIC);
    sqlite3_bind_text(strQuery, 2, name_str.c_str(), name_str.size(), SQLITE_STATIC);
    sqlite3_bind_text(strQuery, 3, dir_str.c_str(), dir_str.size(), SQLITE_STATIC);
    rc = sqlite3_step(strQuery);
    errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
}

void dbop::Database::insert_Histogram(int flag, int fb, int sb, int tb) {
    feat::Histogram histOper(cv::Mat(), flag, fb, sb, tb);
    string hash_str = std::to_string(histOper.getHash());

    char sql[300];
    sprintf(sql, "INSERT INTO Histogram " \
        "VALUES ('%s', %f, %f, %f, %f);", hash_str.c_str(), static_cast<float>(flag), static_cast<float>(fb),
        static_cast<float>(sb), static_cast<float>(tb));
    int rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    errorCheck(rc, zErrMsg);
}

void dbop::Database::insert_Histogram(feat::Histogram hist) {
    vecf vecOper = hist.getVariablesFloat();

    char sql[300];
    sprintf(sql, "INSERT INTO Histogram " \
        "VALUES ('%s', %f, %f, %f, %f);", std::to_string(hist.getHash()).c_str(), vecOper[0], vecOper[1], vecOper[2], vecOper[3]);
    int rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    errorCheck(rc, zErrMsg);
}

void dbop::Database::insert_Edge(int flag, feat::Edge::Canny* edcOper) {
    feat::Edge edgeOper(cv::Mat(), flag, edcOper);
    string hash_str = std::to_string(edgeOper.getHashVariables()[0]);

    sqlite3_stmt* strQuery = NULL;
    int rc = sqlite3_prepare_v2(db, "INSERT INTO edge(hash, flag, edchash) VALUES (?, ?, ?)", -1, &strQuery, NULL);

    if (edcOper != nullptr) {
        string edcHash_str = std::to_string(edcOper->getHash());
        sqlite3_bind_text(strQuery, 1, hash_str.c_str(), hash_str.size(), SQLITE_STATIC);
        sqlite3_bind_int(strQuery, 2, flag);
        sqlite3_bind_text(strQuery, 3, edcHash_str.c_str(), edcHash_str.size(), SQLITE_STATIC);
    }
    else {
        sqlite3_bind_text(strQuery, 1, hash_str.c_str(), hash_str.size(), SQLITE_STATIC);
        sqlite3_bind_int(strQuery, 2, flag);
        sqlite3_bind_null(strQuery,3);
    }

    rc = sqlite3_step(strQuery);
    errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
}

void dbop::Database::insert_Edge(feat::Edge edge) {
    string hash_str = std::to_string(edge.getHashVariables()[0]);

    sqlite3_stmt* strQuery = NULL;
    int rc = sqlite3_prepare_v2(db, "INSERT INTO edge(hash, flag, edchash) VALUES (?, ?, ?)", -1, &strQuery, NULL);

    if (edge.getHashVariables().size() == 2) {
        string edcHash_str = std::to_string(edge.getHashVariables()[1]);
        sqlite3_bind_text(strQuery, 1, hash_str.c_str(), hash_str.size(), SQLITE_STATIC);
        sqlite3_bind_int(strQuery, 2, edge.getEdgeFlag());
        sqlite3_bind_text(strQuery, 3, edcHash_str.c_str(), edcHash_str.size(), SQLITE_STATIC);
    }
    else {
        sqlite3_bind_text(strQuery, 1, hash_str.c_str(), hash_str.size(), SQLITE_STATIC);
        sqlite3_bind_int(strQuery, 2, edge.getEdgeFlag());
        sqlite3_bind_null(strQuery, 3);
    }

    rc = sqlite3_step(strQuery);
    errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
}

void dbop::Database::insert_ImageHistogram(XXH64_hash_t imHash, XXH64_hash_t histHash) {
    char sql[300];
    
    sprintf(sql, "INSERT INTO ImageHistogram " \
        "VALUES ('%s', '%s');", std::to_string(imHash).c_str(), std::to_string(histHash).c_str());

    int rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    errorCheck(rc, zErrMsg);
}

void dbop::Database::insert_Icon(cv::Mat iconMat_src) {
    img::Icon icon(iconMat_src);

    string hash_str = std::to_string(icon.getHash());

    string condition = "hash='" + hash_str + "'";
    if (select_Hash("", "icon", condition).size() == 1)
        return;

    string icon_str = serializeMat(icon.getIconMat());
    const char* iconChar = icon_str.c_str();

    sqlite3_stmt* strQuery = NULL;
    int rc = sqlite3_prepare_v2(db, "INSERT INTO icon(hash, mat) VALUES (?, ?)", -1, &strQuery, NULL);

    sqlite3_bind_text(strQuery, 1, hash_str.c_str(), hash_str.size(), SQLITE_STATIC);
    sqlite3_bind_blob(strQuery, 2, iconChar, strlen(iconChar), SQLITE_STATIC);

    rc = sqlite3_step(strQuery);
    errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
}

void dbop::Database::insert_Icon(img::Icon icon) {
    string hash_str = std::to_string(icon.getHash());
    
    string condition = "hash='" + hash_str + "'";
    if (select_Hash("", "icon", condition).size() == 1)
        return;

    string icon_str = serializeMat(icon.getIconMat());
    const char* iconChar = icon_str.c_str();

    sqlite3_stmt* strQuery = NULL;
    int rc = sqlite3_prepare_v2(db, "INSERT INTO icon(hash, mat) VALUES (?, ?)", -1, &strQuery, NULL);

    sqlite3_bind_text(strQuery, 1, hash_str.c_str(), hash_str.size(), SQLITE_STATIC);
    sqlite3_bind_blob(strQuery, 2, iconChar, strlen(iconChar), SQLITE_STATIC);

    rc = sqlite3_step(strQuery);
    errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
}

void dbop::Database::insert_ImageIcon(XXH64_hash_t imHash, XXH64_hash_t iconHash) {
    char sql[300];

    sprintf(sql, "INSERT INTO ImageIcon " \
        "VALUES ('%s', '%s');", std::to_string(imHash).c_str(), std::to_string(iconHash).c_str());

    int rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    errorCheck(rc, zErrMsg);
}

void dbop::Database::insert_SourceImage(img::Image image) {
    string hash_str = std::to_string(image.getHash());
    string name_str = image.getVariablesString()[0];
    string dir_str = image.getVariablesString()[1];

    sqlite3_stmt* strQuery = NULL;
    int rc = sqlite3_prepare_v2(db, "INSERT INTO SourceImage(hash, name, dir) VALUES (?, ?, ?)", -1, &strQuery, NULL);
    sqlite3_bind_text(strQuery, 1, hash_str.c_str(), hash_str.size(), SQLITE_STATIC);
    sqlite3_bind_text(strQuery, 2, name_str.c_str(), name_str.size(), SQLITE_STATIC);
    sqlite3_bind_text(strQuery, 3, dir_str.c_str(), dir_str.size(), SQLITE_STATIC);
    rc = sqlite3_step(strQuery);
    errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
}

void dbop::Database::insert_EdgeCanny(feat::Edge::Canny canny) {
    feat::Edge edge(cv::Mat(), EDGE_CANNY, &canny);
    insert_Edge(edge);

    string condition = "hash='" + std::to_string(canny.getHash()) + "'";
    if(select_Hash("", "edgecanny", condition).size() == 1)
        return;

    string hash_str = std::to_string(canny.getHash());
    vecf floatVec = canny.getVariablesFloat();
    std::vector<cv::Mat> matVec = canny.getVariablesMat();
    string kernelx = dbop::serializeMat(matVec[0]);
    string kernely = dbop::serializeMat(matVec[1]);

    sqlite3_stmt* strQuery = NULL;
    int rc = sqlite3_prepare_v2(db, "INSERT INTO edgecanny(hash, gausskernelsize, sigma, thigh, tlow, kernelx, kernely) " \
        "VALUES (?, ?, ?, ?, ?, ?, ?)", -1, &strQuery, NULL);

    sqlite3_bind_text(strQuery, 1, hash_str.c_str(), hash_str.size(), SQLITE_STATIC);
    sqlite3_bind_int(strQuery, 2, static_cast<int>(floatVec[0]));
    sqlite3_bind_double(strQuery, 3, static_cast<double>(floatVec[3]));
    sqlite3_bind_double(strQuery, 4, static_cast<double>(floatVec[1]));
    sqlite3_bind_double(strQuery, 5, static_cast<double>(floatVec[2]));
    sqlite3_bind_blob(strQuery, 6, kernelx.c_str(), kernelx.size(), SQLITE_STATIC);
    sqlite3_bind_blob(strQuery, 7, kernely.c_str(), kernely.size(), SQLITE_STATIC);
    rc = sqlite3_step(strQuery);
    errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
}

void dbop::Database::insert_EdgeCanny(int gaussKernelSize, float thigh, float tlow, float sigma, cv::Mat kernelx, cv::Mat kernely) {
    feat::Edge::Canny canny(gaussKernelSize, sigma, thigh, tlow, kernelx, kernely);
    feat::Edge edge(cv::Mat(), EDGE_CANNY, &canny);
    insert_Edge(edge);

    string condition = "hash=" + canny.getHash();
    if (select_Hash("", "edgecanny", condition).size() == 1)
        return;

    string hash_str = std::to_string(canny.getHash());
    string kernelx_srlzd = dbop::serializeMat(kernelx);
    string kernely_srlzd = dbop::serializeMat(kernely);

    sqlite3_stmt* strQuery = NULL;
    int rc = sqlite3_prepare_v2(db, "INSERT INTO edgecanny(hash, gausskernelsize, sigma, thigh, tlow, kernelx, kernely) " \
        "VALUES (?, ?, ?, ?, ?, ?, ?)", -1, &strQuery, NULL);

    sqlite3_bind_text(strQuery, 1, hash_str.c_str(), hash_str.size(), SQLITE_STATIC);
    sqlite3_bind_int(strQuery, 2, gaussKernelSize);
    sqlite3_bind_double(strQuery, 3, static_cast<double>(sigma));
    sqlite3_bind_double(strQuery, 4, static_cast<double>(thigh));
    sqlite3_bind_double(strQuery, 5, static_cast<double>(tlow));
    sqlite3_bind_blob(strQuery, 6, kernelx_srlzd.c_str(), kernelx_srlzd.size(), SQLITE_STATIC);
    sqlite3_bind_blob(strQuery, 7, kernely_srlzd.c_str(), kernely_srlzd.size(), SQLITE_STATIC);
    rc = sqlite3_step(strQuery);
    errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
}

void dbop::Database::insert_ImageEdge(XXH64_hash_t imHash, XXH64_hash_t edgeHash) {
    char sql[300];

    sprintf(sql, "INSERT INTO ImageEdge " \
        "VALUES ('%s', '%s');", std::to_string(imHash).c_str(), std::to_string(edgeHash).c_str());

    int rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    errorCheck(rc, zErrMsg);
}

void dbop::Database::insert_CornerHarris(float radius, float squareSize, float sigmai, float sigmad, float alpha,
    cv::Mat kernelx, cv::Mat kernely) {
    feat::Corner::Harris harris(radius, squareSize, sigmai, sigmad, alpha,
        kernelx, kernely);

    string condition = "hash=" + harris.getHash();
    if (select_Hash("", "cornerharris", condition).size() == 1)
        return;

    string hash_str = std::to_string(harris.getHash());
    string kernelx_srlzd = dbop::serializeMat(kernelx);
    string kernely_srlzd = dbop::serializeMat(kernely);

    sqlite3_stmt* strQuery = NULL;
    int rc = sqlite3_prepare_v2(db, "INSERT INTO cornerHarris(hash, radius, squareSize, sigmai, sigmad, alpha, kernelx, kernely) " \
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?)", -1, &strQuery, NULL);

    sqlite3_bind_text(strQuery, 1, hash_str.c_str(), hash_str.size(), SQLITE_STATIC);
    sqlite3_bind_int(strQuery, 2, static_cast<int>(round(radius)));
    sqlite3_bind_int(strQuery, 3, static_cast<int>(round(squareSize)));
    sqlite3_bind_double(strQuery, 4, static_cast<double>(sigmai));
    sqlite3_bind_double(strQuery, 5, static_cast<double>(sigmad));
    sqlite3_bind_double(strQuery, 6, static_cast<double>(alpha));
    sqlite3_bind_blob(strQuery, 7, kernelx_srlzd.c_str(), kernelx_srlzd.size(), SQLITE_STATIC);
    sqlite3_bind_blob(strQuery, 8, kernely_srlzd.c_str(), kernely_srlzd.size(), SQLITE_STATIC);
    rc = sqlite3_step(strQuery);
    errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
}

void dbop::Database::insert_CornerHarris(feat::Corner::Harris harris) {
    string condition = "hash=" + harris.getHash();
    if (select_Hash("", "cornerharris", condition).size() == 1)
        return;

    string hash_str = std::to_string(harris.getHash());
    string kernelx_srlzd = dbop::serializeMat(harris.getVariablesMat()[0]);
    string kernely_srlzd = dbop::serializeMat(harris.getVariablesMat()[1]);

    sqlite3_stmt* strQuery = NULL;
    int rc = sqlite3_prepare_v2(db, "INSERT INTO cornerHarris(hash, radius, squareSize, sigmai, sigmad, alpha, kernelx, kernely) " \
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?)", -1, &strQuery, NULL);

    sqlite3_bind_text(strQuery, 1, hash_str.c_str(), hash_str.size(), SQLITE_STATIC);
    sqlite3_bind_int(strQuery, 2, static_cast<int>(round(harris.getVariablesFloat()[0])));
    sqlite3_bind_int(strQuery, 3, static_cast<int>(round(harris.getVariablesFloat()[1])));
    sqlite3_bind_double(strQuery, 4, static_cast<double>(harris.getVariablesFloat()[2]));
    sqlite3_bind_double(strQuery, 5, static_cast<double>(harris.getVariablesFloat()[3]));
    sqlite3_bind_double(strQuery, 6, static_cast<double>(harris.getVariablesFloat()[4]));
    sqlite3_bind_blob(strQuery, 7, kernelx_srlzd.c_str(), kernelx_srlzd.size(), SQLITE_STATIC);
    sqlite3_bind_blob(strQuery, 8, kernely_srlzd.c_str(), kernely_srlzd.size(), SQLITE_STATIC);
    rc = sqlite3_step(strQuery);
    errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
}

void dbop::Database::insert_Corner(int flag, int numberOfScales, float scaleRatio, feat::Corner::Harris* harrisOper) {
    feat::Corner cornerOper(cv::Mat(), harrisOper, flag, numberOfScales, scaleRatio);
    string hash_str = std::to_string(cornerOper.getHashVariables()[0]);

    sqlite3_stmt* strQuery = NULL;
    int rc = sqlite3_prepare_v2(db, "INSERT INTO corner(hash, flag, cdhHash, numberofScales, scaleRatio)" \
        "VALUES (?, ?, ?, ?, ?)", -1, &strQuery, NULL);

    string cdhHash_str = std::to_string(harrisOper->getHash());
    sqlite3_bind_text(strQuery, 1, hash_str.c_str(), hash_str.size(), SQLITE_STATIC);
    sqlite3_bind_int(strQuery, 2, flag);
    sqlite3_bind_text(strQuery, 3, cdhHash_str.c_str(), cdhHash_str.size(), SQLITE_STATIC);
    sqlite3_bind_int(strQuery, 4, numberOfScales);
    sqlite3_bind_double(strQuery, 5, static_cast<double>(scaleRatio));

    rc = sqlite3_step(strQuery);
    errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
}

void dbop::Database::insert_Corner(feat::Corner corner) {
    string hash_str = std::to_string(corner.getHashVariables()[0]);

    sqlite3_stmt* strQuery = NULL;
    int rc = sqlite3_prepare_v2(db, "INSERT INTO corner(hash, flag, cdhHash, numberofScales, scaleRatio)" \
        "VALUES (?, ?, ?, ?, ?)", -1, &strQuery, NULL);

    string cdhHash_str = std::to_string(corner.getHashVariables()[1]);
    sqlite3_bind_text(strQuery, 1, hash_str.c_str(), hash_str.size(), SQLITE_STATIC);
    sqlite3_bind_int(strQuery, 2, corner.getIntVariables()[0]);
    sqlite3_bind_text(strQuery, 3, cdhHash_str.c_str(), cdhHash_str.size(), SQLITE_STATIC);
    sqlite3_bind_int(strQuery, 4, corner.getIntVariables()[1]);
    sqlite3_bind_double(strQuery, 5, static_cast<double>(corner.getScaleRatio()));

    rc = sqlite3_step(strQuery);
    errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
}

void dbop::Database::insert_ImageCorner(XXH64_hash_t imHash, XXH64_hash_t cornerHash) {
    char sql[300];

    sprintf(sql, "INSERT INTO ImageCorner " \
        "VALUES ('%s', '%s');", std::to_string(imHash).c_str(), std::to_string(cornerHash).c_str());

    int rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    errorCheck(rc, zErrMsg);
}

void dbop::Database::insert_Similarity(iop::Comparison comp) {
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

void dbop::Database::insert_DestinationImage(img::Image image) {
	string hash_str = std::to_string(image.getHash());
	string name_str = image.getVariablesString()[0];
	string dir_str = image.getVariablesString()[1];

	sqlite3_stmt* strQuery = NULL;
	int rc = sqlite3_prepare_v2(db, "INSERT INTO DestImage(hash, name, dir) VALUES (?, ?, ?)", -1, &strQuery, NULL);
	sqlite3_bind_text(strQuery, 1, hash_str.c_str(), hash_str.size(), SQLITE_STATIC);
	sqlite3_bind_text(strQuery, 2, name_str.c_str(), name_str.size(), SQLITE_STATIC);
	sqlite3_bind_text(strQuery, 3, dir_str.c_str(), dir_str.size(), SQLITE_STATIC);
	rc = sqlite3_step(strQuery);
	errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
}

void dbop::Database::insert_WeightVector(iop::WeightVector wvec) {
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
    for (string i : paramVec[0]) {
        attributes.append(i);
        attributes.append(",");
        att++;
    }
    attributes.erase(attributes.end() - 1);

    string tables = "";
    for (string i : paramVec[1]) {
        tables.append(i);
        tables.append(",");
    }
    tables.erase(tables.end() - 1);

	string conditions = "";
	if(!paramVec[2].empty())
		conditions = paramVec[2][0];

	string statement;
	if (conditions != "")
		statement = "SELECT " + attributes + " FROM " + tables + " WHERE " + conditions + ";";
	else
		statement = "SELECT " + attributes + " FROM " + tables + ";";
    int rc = sqlite3_prepare_v2(db, statement.c_str(), -1 , &strQuery, NULL);
    /*sqlite3_bind_text(strQuery, 1, attributes.c_str(), attributes.size(), SQLITE_STATIC);
    sqlite3_bind_text(strQuery, 2, tables.c_str(), tables.size(), SQLITE_STATIC);
    sqlite3_bind_text(strQuery, 3, conditions.c_str(), conditions.size(), SQLITE_STATIC);*/

    std::vector<std::vector<string>> hashVec;
    int row = 0;
    bool done = false;
    for (int i = 0; i < att; i++)
        hashVec.push_back(std::vector<string>{});

    while (!done) {
        switch (int rc = sqlite3_step(strQuery)) {
        case SQLITE_ROW:
            for (int i = 0; i < att; i++) {
                if (sqlite3_column_bytes(strQuery, 0 + i) == 0);                    
                else
                    hashVec[i].push_back(reinterpret_cast<const char*>(sqlite3_column_text(strQuery, 0 + i)));
            }
            row++;
            break;
        case SQLITE_DONE:
            done = true;
            return hashVec;
        default:
            try {
                errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));                
            }
            catch(std::exception e){
                std::cerr << e.what();
                return std::vector<std::vector<string>>{};
            }
        }
    }
    return std::vector<std::vector<string>>{};
}

std::vector<string> dbop::Database::select_Hash(string hashAbb, string className, string condition) {
    sqlite3_stmt* strQuery = NULL;

    int rc;
    string stmt;
    if (condition != "") {
        stmt = "SELECT " + hashAbb + "hash" + " FROM " + className + " WHERE " + condition + ";";
        rc = sqlite3_prepare_v2(db, stmt.c_str(), -1, &strQuery, NULL);
    }
    else {
        stmt = "SELECT " + hashAbb + "hash" + " FROM " + className;
        rc = sqlite3_prepare_v2(db, stmt.c_str(), -1, &strQuery, NULL);
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

void dbop::Database::delete_GENERAL(string table, string conditions) {
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

string dbop::serializeMat(cv::Mat operand) {
    uchar* pixelPtr = operand.data;
	
	std::mutex m;
	std::vector<std::ostringstream> streamVec;
	int currentNum = 0;
	std::vector<int> hulo;
	hulo.push_back(0);
	hulo.push_back(0);
	hulo.push_back(0);
	hulo.push_back(0);

	auto task = [&streamVec, &pixelPtr, &m, &operand, &currentNum, &hulo](int i, int notlast, int perThread) {
		std::ostringstream thread_stream;
		if (i == 0) {
			thread_stream << operand.dims << " ";
			for (int i = 0; i < operand.dims; i++) {
				thread_stream << operand.size[i] << " ";
			}
			thread_stream << operand.type() << " ";
		}
		for (int j = notlast * i; j < perThread * (i + 1); j++) {
			thread_stream << static_cast<float>(pixelPtr[j]) << " ";
			hulo[i]++;
		}
		while (i != currentNum);
		m.lock();
		streamVec.push_back(std::move(thread_stream));
		currentNum++;
		m.unlock();
	};

	std::vector<std::thread*> threadVec;

	int threadNum = std::thread::hardware_concurrency();
	int remainer = operand.total() * operand.elemSize() % threadNum;
	int notlast = floor(operand.total() * operand.elemSize() / threadNum);
	int last = notlast + remainer;

	for (int i = 0; i < threadNum; i++) {
		int perThread = notlast;
		if (i == threadNum - 1)
			perThread = last;
		std::thread* thr = new std::thread(task, i, notlast, perThread);
		threadVec.push_back(thr);
	}

	while (streamVec.size() != 4);

	for (auto &i : threadVec) {
		i->join();
		delete(i);
	}

	string result = "";
	for (int i = 0; i < 4; i++) {
		result += streamVec[i].str();
	}
	
    return result;
}

cv::Mat dbop::deserializeMat(string operand){
	std::mutex m;
	int currentNum = 0, initialNum = 0;
	int matDims, matType;
	int *matSize;

	cv::Mat matOper;
	std::vector<std::vector<uchar>> ucharVec;

	auto task = [&matOper, &matDims, &matType, &matSize, &m, &operand, &currentNum, 
		&initialNum, &ucharVec](int i, int offset, int move) {
		std::istringstream thread_stream(operand.substr(offset, move - offset));
		while (i != initialNum);
		if (i == 0) {
			thread_stream >> matDims;
			matSize = new int[matDims];
			for (int j = 0; j < matDims; j++) {
				thread_stream >> matSize[j];
			}
			thread_stream >> matType;
			matOper = cv::Mat(matDims, matSize, matType);
			initialNum = 1;
		}
		else
			initialNum++;
		
		std::vector<uchar> pixelPtr;
		float foper;
		while(thread_stream >> foper){
			pixelPtr.push_back(foper);
		}
		while (i != currentNum);
		m.lock();
		ucharVec.push_back(pixelPtr);
		currentNum++;
		m.unlock();
	};

	int threadNum = std::thread::hardware_concurrency();
	int remainer = operand.length() % threadNum;
	int notlast = floor(operand.length() / threadNum);

	std::vector<std::thread*> threadVec;

	std::vector<std::pair<int, int>> offsetVec;
	for (int i = 0; i < threadNum; i++) {
		int move = 0;
		int offset = 0;
		if (i != 0) {
			offset = offsetVec[i - 1].second;
		}
		move = offset + notlast;
		while (i != threadNum - 1 && operand[move] != ' ') {
			move++;
		}
		if(i == threadNum - 1)
			offsetVec.push_back(std::make_pair(offset, operand.length()));
		else
			offsetVec.push_back(std::make_pair(offset, move));
	}
	for (int i = 0; i < threadNum; i++) {
		int offset = offsetVec[i].first;
		int move = offsetVec[i].second;
		
		std::thread* thr = new std::thread(task, i, offset, move);
		threadVec.push_back(thr);
	}

	while (currentNum != 4);

	for (auto& i : threadVec) {
		i->join();
		delete(i);
	}

	int sum = 0;
	for (int i = 0; i < ucharVec.size(); i++) {
		std::move(ucharVec[i].begin(), ucharVec[i].end(), matOper.data + sum);
		sum += ucharVec[i].size();
	}

	delete[](matSize);

	gen::imageTesting(matOper, "test2");

    return matOper;
}

template <typename T>
std::vector<T> matToVector(cv::Mat operand) {   //https://stackoverflow.com/questions/26681713/convert-mat-to-array-vector-in-opencv
    std::vector<T> matvec;
    cv::uint8_t* pixelPtr = (uint8_t*)operand.data;
    int cn = operand.channels();
    cv::Scalar_<uint8_t> bgrPixel;

    matvec.push_back((T)operand.rows);
    matvec.push_back((T)operand.cols);
    matvec.push_back((T)operand.type());

    for (int i = 0; i < operand.rows; i++) {
        for (int j = 0; j < operand.cols; j++) {
            bgrPixel.val[0] = pixelPtr[i * operand.cols * cn + j * cn + 0]; // B
            bgrPixel.val[1] = pixelPtr[i * operand.cols * cn + j * cn + 1]; // G
            bgrPixel.val[2] = pixelPtr[i * operand.cols * cn + j * cn + 2]; // R
            matvec.push_back((T)bgrPixel.val[0]);
            matvec.push_back((T)bgrPixel.val[1]);
            matvec.push_back((T)bgrPixel.val[2]);
        }
    }
    return matvec;
}

template <typename T>
cv::Mat vectorToMat(std::vector<T> operand) { //PARALLELLISE

    std::vector<uint16_t> uintoperator(4, 0);
    int row, col, type;

    row = (int)operand[0];
    col = (int)operand[1];
    type = (int)operand[2];

    cv::Mat matoper(row, col, type);
    cv::Vec3b* ptr;

    for (int i = 0; i < row; i++) {
        ptr = matoper.ptr<cv::Vec3b>(i);
        for (int j = 0; j < col; j++) {
            for (int k = 0; k < uintoperator.size(); k++) {
                uintoperator[k] = operand[3 + i + j + k];
            }
            /*cv::Vec3b vec3boper({ uintoperator });
            ptr[j] = vec3boper;*/
        }
    }
    return matoper;
}