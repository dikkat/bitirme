#include "image.h"

img::Image::Image(std::string imgdir, int flag) {
	imageMat = readImageFile(imgdir, flag);
    name = buildImageName(imgdir);
    dir = imgdir;
    hash = feat::Hash::setHash(std::vector<std::string>{dir});
}

const std::string img::Image::getImageName() {
    return name;
}

cv::Mat img::Image::getImageMat() {
    return imageMat;
}

//NAME, DIR
std::vector<std::string> img::Image::getVariablesString() {
    return std::vector<std::string>{name, dir};
}

XXH64_hash_t img::Image::getHash() {
    return hash;
}

std::string img::buildImageName(std::string imgdir) {
    int j = 0;
    for (int i = 0; i < imgdir.size(); i++) {
        if (imgdir[i] == '/' || imgdir[i] == '\\') {
            j = i;
        }
    }
    std::string imnm = imgdir.substr(j + 1);
    return imnm;
}

void img::Image::setImageDirectory(std::string imgdir) {
    name = buildImageName(imgdir);
    dir = imgdir;
}

cv::Mat img::Image::readImageFile(std::string imgdir, int flag) {
	if (flag != cv::IMREAD_UNCHANGED && flag != cv::IMREAD_COLOR && flag != cv::IMREAD_GRAYSCALE)
		throw std::exception("Illegal image read flag.1");
	cv::Mat matoperator;
	matoperator = cv::imread(imgdir, flag);
	if (matoperator.dims == 0)
		throw std::exception("Can't read the file.");
	return matoperator;
}

img::Icon::Icon(cv::Mat iconMat_src) {
    cv::Mat iconMatOper = iconMat_src.clone();

    int maximumWidth = 100;
    int maximumHeight = MIN(maximumWidth * iconMatOper.rows / iconMatOper.cols, maximumWidth);
    cv::resize(iconMatOper, iconMatOper, cv::Size(maximumWidth, maximumHeight));

    hash = feat::Hash::setHash(&std::vector<cv::Mat>{iconMatOper}, nullptr);
    iconMat = iconMatOper;
}

cv::Mat img::Icon::getIconMat() {
    return iconMat;
}

XXH64_hash_t img::Icon::getHash() {
    return hash;
}

std::vector<img::Image> img::readImageFolder(std::string imageFolderDirectory, int flag, bool all, int number) {
    if (imageFolderDirectory.find(".") != std::string::npos)
        throw std::exception("Illegal folder path.");
    else if (flag != cv::IMREAD_UNCHANGED && flag != cv::IMREAD_COLOR && flag != cv::IMREAD_GRAYSCALE)
        throw std::exception("Illegal image read flag.2");
    img::Image imgoperator;
    std::string pathstring;
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

std::string img::typeToString(int type) {
    std::string r;

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

void img::displayImage(cv::Mat imgmat) {
    cv::namedWindow("testing", cv::WINDOW_AUTOSIZE);
    cv::imshow("testing", imgmat);
}