#include "DatabaseOperations.h"

MYSQL* dbop::connectToDatabaseMYSQL(std::string server, std::string user, std::string password, std::string database) {
    MYSQL* connect;
    connect = mysql_init(NULL);
    if (!connect) {
        return false;
    }
    connect = mysql_real_connect(connect, server.c_str(), user.c_str(), password.c_str(), database.c_str(), 0, NULL, 0);
    if (connect) {
        return connect;
    }
    else {
        return false;
    }
}

void dbop::disconnectFromDatabaseMYSQL(MYSQL* connect) {
    if (connect) {
        mysql_close(connect);
        return;
    }
    else
        return;
}

bool dbop::connectToDatabaseSQLITE(const char* file) {
    sqlite3 db();
    
    return true;
}

std::string dbop::serializeMat(cv::Mat operand) {
    std::ostringstream srlzstrstream;
    cv::uint8_t* pixelPtr = (uint8_t*)operand.data;
    int cn = operand.channels();
    cv::Scalar_<uint8_t> bgrPixel;

    srlzstrstream << operand.rows << " " << operand.cols << " " << operand.type() << " ";
    for (int i = 0; i < operand.rows; i++) {
        for (int j = 0; j < operand.cols; j++) {
            bgrPixel.val[0] = pixelPtr[i * operand.cols * cn + j * cn + 0]; // B
            bgrPixel.val[1] = pixelPtr[i * operand.cols * cn + j * cn + 1]; // G
            bgrPixel.val[2] = pixelPtr[i * operand.cols * cn + j * cn + 2]; // R
            srlzstrstream << (int)bgrPixel.val[0] << " " << (int)bgrPixel.val[1] << " " << (int)bgrPixel.val[2] << " ";
        }
    }

    std::string srlzdstr = srlzstrstream.str();
    return srlzdstr;
}

cv::Mat dbop::deserializeMat(std::string operand){
    uint16_t uintoperator1, uintoperator2, uintoperator3;
    std::istringstream desrlzstrstream(operand);
    int row, col, type;

    desrlzstrstream >> row;
    desrlzstrstream >> col;
    desrlzstrstream >> type;

    cv::Mat matoper(row, col, CV_8UC3);
    cv::Vec3b* ptr;

    for (int i = 0; i < row; i++) {
         ptr = matoper.ptr<cv::Vec3b>(i);
        for (int j = 0; j < col; j++) {
            desrlzstrstream >> uintoperator1;
            desrlzstrstream >> uintoperator2;
            desrlzstrstream >> uintoperator3;
            ptr[j] = cv::Vec3b(uintoperator1, uintoperator2, uintoperator3);
        }
    }

    return matoper;
}