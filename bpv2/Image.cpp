#include "image.h"

img::Image::Image() : dir(""), name(""), hash(XXH64(dir.c_str(), dir.size(), NULL)) {}

img::Image::Image(const string& imgdir, int flag) {
	if(flag != IMG_EMPTY)
		imageMat = readImageFile(imgdir, flag);
    name = buildImageName(imgdir);
    dir = imgdir;
	correctDir();
    hash = feat::Hash::setHash(std::vector<string>{dir});
}

img::Image::Image(const img::Image& other) {
	*this = other;
}

const string img::Image::getImageName() {
    return name;
}

cv::Mat img::Image::getImageMat() {
    return imageMat;
}

//NAME, DIR
std::vector<string> img::Image::getVariablesString() const {
    return std::vector<string>{name, dir};
}

XXH64_hash_t img::Image::getHash() const {
    return hash;
}

string img::buildImageName(const string& imgdir) {
    int j = 0;
    for (int i = 0; i < imgdir.size(); i++) {
        if (imgdir[i] == '/' || imgdir[i] == '\\') {
            j = i;
        }
    }
    string imnm = imgdir.substr(j + 1);
    return imnm;
}

void img::Image::setImageDirectory(const string& imgdir) {
    name = buildImageName(imgdir);
    dir = imgdir;
}

cv::Mat img::Image::readImageFile(const string& imgdir, int flag) {
	if (flag != cv::IMREAD_UNCHANGED && flag != cv::IMREAD_COLOR && flag != cv::IMREAD_GRAYSCALE)
		throw std::exception("Illegal image read flag.1");
	cv::Mat matoperator;
	matoperator = cv::imread(imgdir, flag);
	if (matoperator.dims == 0 || matoperator.data == NULL)
		throw std::exception("Can't read the file.");
	return matoperator;
}

void img::Image::correctDir() {
	std::replace_if(this->dir.begin(), this->dir.end(), [](char c) { return c == '/'; }, '/');
}

std::vector<img::Image> img::readImageFolder(const string& imageFolderDirectory, int flag, bool all, int number) {
    if (imageFolderDirectory.find(".") != string::npos)
        throw std::exception("Illegal folder path.");
    else if (flag != cv::IMREAD_UNCHANGED && flag != cv::IMREAD_COLOR && flag != cv::IMREAD_GRAYSCALE)
        throw std::exception("Illegal image read flag.2");
    img::Image imgoperator;
    string pathstring;
    std::vector<img::Image> vecoperator;
    int iter = 0;
    for (const auto& entry : std::filesystem::directory_iterator(imageFolderDirectory)) {
        pathstring = entry.path().u8string();
        imgoperator = img::Image::Image(pathstring, flag);
        if (imgoperator.getImageMat().data == NULL) {
            continue;
        }
        vecoperator.push_back(imgoperator);
        iter++;
        if (iter == number && !all)
            break;
    }
    return vecoperator;
}

string img::typeToString(int type) {
    string r;

    uchar depth = type & CV_MAT_DEPTH_MASK;
    uchar chans = 1 + (type >> CV_CN_SHIFT);

    switch (depth) {
    case CV_8U:  r = "8U"; break;
    case CV_8S:  r = "8S"; break;
    case CV_16U: r = "16U"; break;
    case CV_16S: r = "16S"; break;
    case CV_32S: r = "32S"; break;
    case CV_32F: r = "32F"; break;
    case CV_64F: r = "64F"; break;
    default:     r = "User"; break;
    }

    r += "C";
    r += (chans + '0');

    return r;
}

void img::displayImage(const cv::Mat& imgmat) {
    cv::namedWindow("testing", cv::WINDOW_AUTOSIZE);
    cv::imshow("testing", imgmat);
}