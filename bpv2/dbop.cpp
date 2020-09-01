#include "dbop.h"

void dbop::initializeDatabase() {
    sqlite3* db;
    int rc;

    rc = sqlite3_open("bitirme.db", &db);

    if (rc) {
        throw std::exception(("Can't open database: %s\n", sqlite3_errmsg(db)));
    }

    initializeTables(db);
}

void dbop::errorCheck(int rc, char* zErrMsg) {
    if (rc != SQLITE_OK) {
        throw std::exception(("SQL error: %s\n", zErrMsg));
        sqlite3_free(zErrMsg);
    }
}

void dbop::initializeTables(sqlite3* db) {
    const char* data = "Callback function called";
    char* zErrMsg = 0;
    char* sql;
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
    }
    errorCheck(rc,zErrMsg);

    sql = "SELECT name FROM sqlite_master WHERE type='table' and name = 'Histogram';";
    rc = sqlite3_get_table(db, sql, &resultp, &row, &col, &zErrMsg);
    errorCheck(rc,zErrMsg);
    if (row == 0) {
        sql = "CREATE TABLE Histogram(" \
            "histHash TEXT PRIMARY KEY NOT NULL," \
            "fbin INT NOT NULL," \
            "sbin INT NOT NULL," \
            "tbin INT NOT NULL," \
            "flag INT NOT NULL);";
        rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    }
    errorCheck(rc, zErrMsg);

    sql = "SELECT name FROM sqlite_master WHERE type='table' and name = 'EdgeCanny';";
    rc = sqlite3_get_table(db, sql, &resultp, &row, &col, &zErrMsg);
    errorCheck(rc,zErrMsg);
    if (row == 0) {
        sql = "CREATE TABLE EdgeCanny(" \
            "edcHash TEXT PRIMARY KEY NOT NULL," \
            "gaussKernelSize INT NOT NULL," \
            "sigma INT NOT NULL," \
            "thigh INT NOT NULL," \
            "tlow INT NOT NULL," \
            "kernelx BLOB NOT NULL," \
            "kernely BLOB NOT NULL);";
        rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    }
    errorCheck(rc,zErrMsg);

    sql = "SELECT name FROM sqlite_master WHERE type='table' and name = 'Edge';";
    rc = sqlite3_get_table(db, sql, &resultp, &row, &col, &zErrMsg);
    errorCheck(rc,zErrMsg);
    if (row == 0) {
        sql = "CREATE TABLE Edge(" \
            "edgeHash TEXT PRIMARY KEY NOT NULL," \
            "flag INT NOT NULL," \
            "edcHash TEXT," \
            "FOREIGN KEY (edcHash) REFERENCES EdgeCanny(edcHash)" \
            "ON DELETE CASCADE);";
        rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    }
    errorCheck(rc,zErrMsg);

    sql = "SELECT name FROM sqlite_master WHERE type='table' and name = 'CornerHarris';";
    rc = sqlite3_get_table(db, sql, &resultp, &row, &col, &zErrMsg);
    errorCheck(rc,zErrMsg);
    if (row == 0) {
        sql = "CREATE TABLE CornerHarris(" \
            "cdhHash TEXT PRIMARY KEY NOT NULL," \
            "radius INT NOT NULL," \
            "squareSize INT NOT NULL," \
            "sigmai INT NOT NULL," \
            "sigmad INT NOT NULL," \
            "alpha INT NOT NULL," \
            "kernelx BLOB NOT NULL," \
            "kernely BLOB NOT NULL);";
        rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    }
    errorCheck(rc,zErrMsg);

    sql = "SELECT name FROM sqlite_master WHERE type='table' and name = 'Corner';";
    rc = sqlite3_get_table(db, sql, &resultp, &row, &col, &zErrMsg);
    errorCheck(rc,zErrMsg);
    if (row == 0) {
        sql = "CREATE TABLE Corner(" \
            "cornerHash TEXT PRIMARY KEY NOT NULL," \
            "flag INT NOT NULL," \
            "cdhHash TEXT," \
            "numberOfScales INT NOT NULL," \
            "scaleRatio INT," \
            "FOREIGN KEY (cdhHash)" \
            "REFERENCES EdgeCanny(cdhHash)" \
            "ON DELETE CASCADE);";
        rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    }
    errorCheck(rc,zErrMsg);

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
    }
    errorCheck(rc,zErrMsg);

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
    }
    errorCheck(rc,zErrMsg);

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
    }
    errorCheck(rc,zErrMsg);
}

static int dbop::callback(void* data, int argc, char** argv, char** azColName) {
    int i;
    fprintf(stderr, "%s: ", (const char*)data);

    for (i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }

    printf("\n");
    return 0;
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
