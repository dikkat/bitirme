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
            "similarity INT," \
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
    int rc = sqlite3_prepare_v2(db, "INSERT INTO image(hash, iconmat, name, dir) VALUES (?, ?, ?)", -1, &strQuery, NULL);
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

/*[0]ATTRIBUTES [1]TABLES [2]CONDITIONS*/
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

    string conditions = paramVec[2][0];

    string statement = "SELECT " + attributes + " FROM " + tables + " WHERE " + conditions + ";";
    int rc = sqlite3_prepare_v2(db, statement.c_str(), -1, &strQuery, NULL);
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
        delete_GENERAL(std::vector<string>{"SourceImage"});
        throw std::exception("Multiple source images, somehow. Pretty much impossible error. Source image table cleared, load source image again even if it's still there.");
    }
    else if (row == 0) {
        throw std::exception("Unable to locate source image. Load image to source first.");
    }
    else {
        return img::Image(imgVec[0], cv::IMREAD_COLOR);
    }
    
}

void dbop::Database::delete_GENERAL(std::vector<string> tableVec, string conditions) {
    sqlite3_stmt* strQuery = NULL;

    string tables = "";
    for (string i : tableVec) {
        tables.append(i);
        tables.append(",");
    }
    tables.erase(tables.end() - 1);

    string statement;
    if (conditions != "")
        statement = "DELETE FROM " + tables + " WHERE " + conditions + ";";
    else
        statement = "DELETE FROM " + tables + ";";
    int rc = sqlite3_prepare_v2(db, statement.c_str(), -1, &strQuery, NULL);
    errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
    rc = sqlite3_step(strQuery);
    errorCheck(rc, const_cast<char*>(sqlite3_errmsg(db)));
}

string dbop::serializeMat(cv::Mat operand) {
    std::ostringstream srlzstr_stream;
    uchar* pixelPtr = operand.data;

    srlzstr_stream << operand.dims << " ";
    for (int i = 0; i < operand.dims; i++) {
        srlzstr_stream << operand.size[i] << " ";
    }
    srlzstr_stream << operand.type() << " ";

    for (int i = 0; i < operand.total() * operand.elemSize(); i++) {
        srlzstr_stream << static_cast<float>(pixelPtr[i]) << " ";
    }
    string srlzd_str = srlzstr_stream.str();
    return srlzd_str;
}

cv::Mat dbop::deserializeMat(string operand){
    std::istringstream desrlzstr_stream(operand);
    int mdims, mtype;

    desrlzstr_stream >> mdims;
    int* msize = new int[mdims];
    for (int i = 0; i < mdims; i++)
        desrlzstr_stream >> msize[i];
    desrlzstr_stream >> mtype;

    cv::Mat matoper(mdims, msize, mtype);
    delete [] msize;

    uchar* pixelPtr = matoper.data;
    float cnpixoper;
    
    for (int i = 0; i < matoper.total() * matoper.elemSize(); i++) {
        desrlzstr_stream >> cnpixoper;
        pixelPtr[i] = cnpixoper;
    }

    return matoper;
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