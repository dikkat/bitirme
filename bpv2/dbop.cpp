#include "dbop.h"

extern dbop::Database dbObj = dbop::Database("bitirme.db");

dbop::Database::Database(std::string dbName) {
    initializeDatabase(dbName);
}

void dbop::Database::initializeDatabase(std::string dbName) {
    sqlite3* db;
    int rc;

    rc = sqlite3_open(dbName.c_str(), &db);

    if (rc) {
        throw std::exception(("Can't open database: %s\n", sqlite3_errmsg(db)));
    }

    dbPtr = db;

    initializeTables();
}

void dbop::Database::errorCheck(int rc, char* zErrMsg) {
    if (rc != SQLITE_OK) {
        throw std::exception(("SQL error: %s\n", zErrMsg));
        sqlite3_free(zErrMsg);
    }
}

void dbop::Database::initializeTables() {
    sqlite3* db = dbPtr;
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
            "iconmat BLOB NOT NULL," \
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
            "cdhHash TEXT," \
            "numberOfScales INT NOT NULL," \
            "scaleRatio REAL," \
            "FOREIGN KEY (cdhHash)" \
            "REFERENCES EdgeCanny(hash)" \
            "ON DELETE CASCADE);";
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

void dbop::Database::insert_Image(std::string dir) {
    XXH64_hash_t hash = feat::Hash::setHash(std::vector<std::string>{dir});
    std::string hash_str = std::to_string(hash);
    std::string name_str = img::buildImageName(dir);

    cv::Mat iconMat = cv::imread(dir, cv::IMREAD_COLOR);
    cv::resize(iconMat, iconMat, cv::Size(100, 100));
    std::string icon_str = serializeMat(iconMat);
    const char* iconChar = icon_str.c_str();


    sqlite3_stmt* strQuery = NULL;
    int rc = sqlite3_prepare_v2(dbPtr, "INSERT INTO image(hash, iconmat, name, dir) VALUES (?, ?, ?, ?)", -1, &strQuery, NULL);
    sqlite3_bind_text(strQuery, 1, hash_str.c_str(), hash_str.size(), SQLITE_STATIC);
    sqlite3_bind_blob(strQuery, 2, iconChar, strlen(iconChar), SQLITE_STATIC);
    sqlite3_bind_text(strQuery, 3, name_str.c_str(), name_str.size(), SQLITE_STATIC);
    sqlite3_bind_text(strQuery, 4, dir.c_str(), dir.size(), SQLITE_STATIC);
    rc = sqlite3_step(strQuery);
    if (rc != SQLITE_DONE) {
        char errMsg[200];
        sprintf(errMsg, "SQL error in Image: %s with name = %s\n", sqlite3_errmsg(dbPtr), name_str.c_str());
        throw std::exception(errMsg);
        sqlite3_free(zErrMsg);
    }
}

void dbop::Database::insert_Image(img::Image image) {
    cv::Mat iconMat = image.getImageMat();
    cv::resize(iconMat, iconMat, cv::Size(48, 48));
    std::string icon_str = serializeMat(iconMat);

    const char* iconChar = icon_str.c_str();
    std::string hash_str = std::to_string(image.getHash());
    std::string name_str = image.getVariablesString()[0];
    std::string dir_str = image.getVariablesString()[1];
    

    sqlite3_stmt* strQuery = NULL;
    int rc = sqlite3_prepare_v2(dbPtr, "INSERT INTO image(hash, iconmat, name, dir) VALUES (?, ?, ?, ?)", -1, &strQuery, NULL);
    sqlite3_bind_text(strQuery, 1, hash_str.c_str(), hash_str.size(), SQLITE_STATIC);
    sqlite3_bind_blob(strQuery, 2, iconChar, strlen(iconChar), SQLITE_STATIC);
    sqlite3_bind_text(strQuery, 3, name_str.c_str(), name_str.size(), SQLITE_STATIC);
    sqlite3_bind_text(strQuery, 4, dir_str.c_str(), dir_str.size(), SQLITE_STATIC);
    rc = sqlite3_step(strQuery);
    if (rc != SQLITE_DONE) {
        char errMsg[200];
        sprintf(errMsg, "SQL error: %s with name = %s\n", sqlite3_errmsg(dbPtr), name_str.c_str());
        throw std::exception(errMsg);
        sqlite3_free(zErrMsg);
    }
}

void dbop::Database::insert_Histogram(int flag, int fb, int sb, int tb) {
    XXH64_hash_t hash = feat::Hash::setHash(nullptr, &std::vector<float>{static_cast<float>(flag), static_cast<float>(fb),
        static_cast<float>(sb), static_cast<float>(tb)});
    std::string hash_str = std::to_string(hash);

    char sql[200];
    sprintf(sql, "INSERT INTO Histogram " \
        "VALUES ('%s', %f, %f, %f, %f);", hash_str.c_str(), static_cast<float>(flag), static_cast<float>(fb),
        static_cast<float>(sb), static_cast<float>(tb));
    int rc = sqlite3_exec(dbPtr, sql, callback, 0, &zErrMsg);
    errorCheck(rc, zErrMsg);
}

void dbop::Database::insert_Histogram(feat::Histogram hist) {
    std::vector<float> vecOper = hist.getVariablesFloat();

    char sql[200];
    sprintf(sql, "INSERT INTO Histogram " \
        "VALUES ('%s', %f, %f, %f, %f);", std::to_string(hist.getHash()).c_str(), vecOper[0], vecOper[1], vecOper[2], vecOper[3]);
    int rc = sqlite3_exec(dbPtr, sql, callback, 0, &zErrMsg);
    errorCheck(rc, zErrMsg);
}

void dbop::Database::insert_Edge(int flag, feat::Edge::Canny* edcOper) {
    XXH64_hash_t hash = feat::Hash::setHash(nullptr, &std::vector<float>{static_cast<float>(flag)});
    std::string hash_str = std::to_string(hash);

    char sql[200];

    if (edcOper != nullptr) {
        std::string edcHash;
        std::vector<std::string> hashVec;
        hashVec.push_back(std::to_string(hash));
        hashVec.push_back(std::to_string(edcOper->getHash()));
        edcHash = std::to_string(feat::Hash::setHash(hashVec));
        
        sprintf(sql, "INSERT INTO Edge " \
            "VALUES ('%s', %d, '%s');", hash_str.c_str(), flag, edcHash.c_str());
    }

    else
        sprintf(sql, "INSERT INTO Edge " \
            "VALUES ('%s', %d, %s);", hash_str.c_str(), flag, "NULL");

    int rc = sqlite3_exec(dbPtr, sql, callback, 0, &zErrMsg);
    errorCheck(rc, zErrMsg);
}

void dbop::Database::insert_Edge(feat::Edge edge) {
    int flag = edge.getEdgeFlag();
    std::vector<XXH64_hash_t> hashVec = edge.getHashVariables();
    
    char sql[200];

    if(hashVec.size() == 2)
        sprintf(sql, "INSERT INTO Edge " \
            "VALUES ('%s', %d, '%s');", std::to_string(hashVec[0]).c_str(), flag, std::to_string(hashVec[1]).c_str());
    else
        sprintf(sql, "INSERT INTO Edge " \
            "VALUES ('%s', %d, '%s');", std::to_string(hashVec[0]).c_str(), flag, "NULL");

    int rc = sqlite3_exec(dbPtr, sql, callback, 0, &zErrMsg);
    errorCheck(rc, zErrMsg);
}

std::string dbop::serializeMat(cv::Mat operand) {
    std::ostringstream srlzstrstream;
    uchar* pixelPtr = (uchar*)operand.data;
    int cn = operand.channels();
    cv::Scalar_<uchar> cnPixel;

    srlzstrstream << operand.dims << " ";
    for (int i = 0; i < operand.dims; i++) {
        srlzstrstream << operand.size[i] << " ";
    }
    srlzstrstream << operand.type() << " ";

    for (int i = 0; i < operand.total() * operand.elemSize(); i++) {
        srlzstrstream << (float)pixelPtr[i] << " ";
    }
    std::string srlzdstr = srlzstrstream.str();
    return srlzdstr;
}

cv::Mat dbop::deserializeMat(std::string operand){
    std::istringstream desrlzstrstream(operand);
    int mdims, mtype;

    desrlzstrstream >> mdims;
    int* msize = new int[mdims];
    for (int i = 0; i < mdims; i++)
        desrlzstrstream >> msize[i];
    desrlzstrstream >> mtype;

    cv::Mat matoper(mdims, msize, mtype);
    delete [] msize;

    uchar* pixelPtr = (uchar*)matoper.data;
    float cnpixoper;

    for (int i = 0; i < matoper.total() * matoper.elemSize(); i++) {
        desrlzstrstream >> cnpixoper;
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