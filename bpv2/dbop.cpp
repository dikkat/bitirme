#include "dbop.h"

void dbop::initializeDatabase() {
    if (!connectToDatabaseSQLITE())
        throw std::exception("Can't connect to database.");

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


bool dbop::connectToDatabaseSQLITE() {
    sqlite3 *db;
    int rc;

    rc = sqlite3_open("bitirme.db", &db);

    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return false;
    }
    else
        return true;
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
