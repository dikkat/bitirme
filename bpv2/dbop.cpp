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
    char** resultp;
    int row, col, rc;

    sql = "PRAGMA foreign_keys = ON;";

    sql = "SELECT name FROM sqlite_master WHERE type='table' and name = 'Image';";
    rc = sqlite3_get_table(db, sql, &resultp, &row, &col, &zErrMsg);
    errorCheck(rc,zErrMsg);
    if (row == 0) {
        sql = "CREATE TABLE Image(" \
            "imHash TEXT PRIMARY KEY NOT NULL," \
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
            "histHash TEXT PRIMARY KEY NOT NULL," \
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
            "edcHash TEXT PRIMARY KEY NOT NULL," \
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
            "edgeHash TEXT PRIMARY KEY NOT NULL," \
            "flag INT NOT NULL," \
            "edcHash TEXT," \
            "FOREIGN KEY (edcHash)" \
            "REFERENCES EdgeCanny(edcHash)" \
            "ON DELETE CASCADE);";
        rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
        errorCheck(rc, zErrMsg);
    }

    sql = "SELECT name FROM sqlite_master WHERE type='table' and name = 'CornerHarris';";
    rc = sqlite3_get_table(db, sql, &resultp, &row, &col, &zErrMsg);
    errorCheck(rc,zErrMsg);
    if (row == 0) {
        sql = "CREATE TABLE CornerHarris(" \
            "cdhHash TEXT PRIMARY KEY NOT NULL," \
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
            "cornerHash TEXT PRIMARY KEY NOT NULL," \
            "flag INT NOT NULL," \
            "cdhHash TEXT," \
            "numberOfScales INT NOT NULL," \
            "scaleRatio REAL," \
            "FOREIGN KEY (cdhHash)" \
            "REFERENCES EdgeCanny(cdhHash)" \
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
            "REFERENCES Image(imHash)" \
            "ON DELETE CASCADE," \
            "FOREIGN KEY (histHash)" \
            "REFERENCES Histogram(histHash)" \
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
            "REFERENCES Image(imHash)" \
            "ON DELETE CASCADE," \
            "FOREIGN KEY (edgeHash)" \
            "REFERENCES Edge(edgeHash)" \
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
            "REFERENCES Image(imHash)" \
            "ON DELETE CASCADE," \
            "FOREIGN KEY (cornerHash)" \
            "REFERENCES Edge(cornerHash)" \
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
    std::string name = img::buildImageName(dir);

    char sqlel[160];
    sprintf(sqlel, "INSERT INTO Image " \
        "VALUES ('%s', '%s', '%s');", hash_str.c_str(), name.c_str(), dir.c_str());
    int rc = sqlite3_exec(dbPtr, sqlel, callback, 0, &zErrMsg);
    errorCheck(rc, zErrMsg);
}

void dbop::Database::insert_Image(img::Image image) {
    std::vector<std::string> vecOper = image.getVariablesString();

    sprintf(sql, "INSERT INTO Image" \
        "VALUES ('%s', '%s', '%s');", std::to_string(image.getHash()).c_str(), vecOper[0].c_str(), vecOper[1].c_str());
    int rc = sqlite3_exec(dbPtr, sql, callback, 0, &zErrMsg);
    errorCheck(rc, zErrMsg);
}

void dbop::Database::insert_Histogram(int flag, int fb, int sb, int tb) {
    XXH64_hash_t hash = feat::Hash::setHash(nullptr, &std::vector<float>{static_cast<float>(flag), static_cast<float>(fb),
        static_cast<float>(sb), static_cast<float>(tb)});
    std::string hash_str = std::to_string(hash);

    sprintf(sql, "INSERT INTO Histogram" \
        "VALUES ('%s', %d, %d, %d, %d);", hash_str.c_str(), flag, fb, sb, tb);
    int rc = sqlite3_exec(dbPtr, sql, callback, 0, &zErrMsg);
    errorCheck(rc, zErrMsg);
}

void dbop::Database::insert_Histogram(feat::Histogram hist) {
    std::vector<float> vecOper = hist.getVariablesFloat();

    sprintf(sql, "INSERT INTO Histogram" \
        "VALUES ('%s', %d, %d, %d, %d);", std::to_string(hist.getHash()).c_str(), vecOper[0], vecOper[1], vecOper[2], vecOper[3]);
    int rc = sqlite3_exec(dbPtr, sql, callback, 0, &zErrMsg);
    errorCheck(rc, zErrMsg);
}

void dbop::Database::insert_Edge(int flag, feat::Edge::Canny* edcOper) {
    XXH64_hash_t hash = feat::Hash::setHash(nullptr, &std::vector<float>{static_cast<float>(flag)});
    std::string hash_str = std::to_string(hash);

    if (edcOper != nullptr) {
        std::string edcHash;
        std::vector<std::string> hashVec;
        hashVec.push_back(std::to_string(hash));
        hashVec.push_back(std::to_string(edcOper->getHash()));
        edcHash = std::to_string(feat::Hash::setHash(hashVec));
        
        sprintf(sql, "INSERT INTO Edge" \
            "VALUES ('%s', %d, '%s');", hash_str.c_str(), flag, edcHash.c_str());
    }

    else
        sprintf(sql, "INSERT INTO Edge" \
            "VALUES ('%s', %d, %s);", hash_str.c_str(), flag, "NULL");
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
