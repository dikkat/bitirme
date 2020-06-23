#include "Image.h"



img::Image::Image(std::string imgdir, int flag) {
	imagemat = readImageFile(imgdir, flag);
    name = buildImageName(imgdir);
    dir = imgdir;
}

img::Image::Image(cv::Mat sourcemat) {
    imagemat = sourcemat.clone();
}

img::Image::Image(cv::Mat sourcemat, std::string imgdir) {
    imagemat = sourcemat.clone();
    name = buildImageName(imgdir);
    dir = imgdir;
}

void img::Image::setImageHist(int fb, int sb, int tb, int flag) {
    imagehist = new Histogram(this, fb, sb, tb, flag);
}

void img::Image::setImageDirectory(std::string imgdir) {
    name = buildImageName(imgdir);
    dir = imgdir;
}

const std::string img::Image::getImageName() {
    return name;
}

cv::Mat img::Image::getImageMat() {
    return this->imagemat;
}

img::Histogram* img::Image::getImageHist() {
    return imagehist;
}

void img::Image::destroyImageHist() {
    this->imagehist == NULL;
}

std::string img::Image::buildImageName(std::string imgdir) {
    int j;
    for (int i = 0; i < imgdir.size(); i++) {
        if (imgdir[i] == '/') {
            j = i;
        }
    }
    j = j + 1;
    std::string imnm = imgdir.substr(j);
    return imnm;
}

cv::Mat img::Image::readImageFile(std::string imgdir, int flag) {
	if (flag != cv::IMREAD_UNCHANGED && flag != cv::IMREAD_COLOR && flag != cv::IMREAD_GRAYSCALE)
		throw std::exception("Illegal image read flag.1");
	cv::Mat matoperator;
	matoperator = cv::imread(imgdir, flag);
	if (matoperator.data == NULL)
		throw std::exception("Can't read the file.");
	return matoperator;
}

img::Histogram::Histogram(Image* srimg, int fb, int sb, int tb, int flag) {
    srcimg = srimg;
    fbin = fb;
    sbin = sb;
    tbin = tb;
    if (flag == 0) {
        if (getSourceMat().channels() == 1)
            histmat = histogramGRAYCalculation(getSourceMat());
        else if (getSourceMat().channels() == 3)
            histmat = histogramBGRCalculation(getSourceMat());
        else
            throw std::exception("Illegal histogram build flag.");
    }
    else if (flag == CALC_BGRHIST)
        histmat = histogramBGRCalculation(getSourceMat());
    else if (flag == CALC_HSVHIST)
        histmat = histogramHSVCalculation(getSourceMat());
    else if (flag == CALC_GRAYHIST)
        histmat = histogramGRAYCalculation(getSourceMat());
    else
        throw std::exception("Illegal histogram build flag.");    
    nhistmat = normalizeMat(histmat, 0, 1);
}

cv::Mat img::Histogram::getHistogramMat() {
    return histmat;
}

cv::Mat img::Histogram::getNormalizedHistogramMat() {
    return nhistmat;
}

cv::Mat img::Histogram::getSourceMat() {
    return srcimg->getImageMat();
}

int* img::Histogram::getBin() {
    int binval[] = { fbin, sbin , tbin};
    return binval;
}

void img::Histogram::setHistogramDisplayImage(int width, int height) {
    histBGR = histogramBGRSeparateCalculation(getSourceMat());
    *histdspimg = createHistogramDisplayImage(histBGR, width, height);
}

cv::Mat img::Histogram::histogramHSVCalculation(cv::Mat sourcemat) {
    cv::MatND hsvbase, histbase;
    cv::cvtColor(sourcemat, hsvbase, cv::COLOR_BGR2HSV);
    int histSize[] = { getBin()[0], getBin()[1], getBin()[2] };

    float h_ranges[] = { 0, 180 };
    float sv_ranges[] = { 0, 256 };
    const float* histRange[] = { h_ranges, sv_ranges, sv_ranges};

    bool uniform = true; bool accumulate = false;
    int channels[] = { 0, 1, 2};

    cv::calcHist(&hsvbase, 1, channels, cv::Mat(), histbase, 3, histSize, histRange, uniform, accumulate);
    return histbase;
}

cv::Mat img::Histogram::histogramBGRCalculation(cv::Mat sourcemat) {
    int histSize[] = { getBin()[0], getBin()[1], getBin()[2] };

    float bgr_ranges[] = { 0, 256 };
    const float* histRange[] = { bgr_ranges, bgr_ranges , bgr_ranges };

    bool uniform = true; bool accumulate = false;
    int channels[] = { 0, 1, 2 };

    cv::MatND histbase;
    cv::calcHist(&sourcemat, 1, channels, cv::Mat(), histbase, 3, histSize, histRange, uniform, accumulate);
    return histbase;
}

cv::Mat img::Histogram::histogramGRAYCalculation(cv::Mat sourcemat) {
    int histSize[] = { getBin()[0] };

    cv::Mat greyMat;
    if (sourcemat.channels() == 3)
        cv::cvtColor(sourcemat, greyMat, cv::COLOR_BGR2GRAY);

    float gray_range[] = { 0, 256 };
    const float* histRange[] = { gray_range };

    bool uniform = true; bool accumulate = false;
    int channels[] = { 0 };

    cv::MatND histbase;
    cv::calcHist(&sourcemat, 1, channels, cv::Mat(), histbase, 1, histSize, histRange, uniform, accumulate);
    return histbase;
}

cv::Mat img::Histogram::normalizeMat(cv::Mat sourcemat, float alpha, float beta) {
    cv::Mat nhist;
    normalize(sourcemat, nhist, alpha, beta, cv::NORM_MINMAX, -1, cv::Mat());
    return nhist;
}

std::vector<cv::Mat> img::Histogram::histogramBGRSeparateCalculation(cv::Mat sourcemat) {
    std::vector<cv::Mat> bgr_planes;
    cv::split(sourcemat, bgr_planes);

    int histSize = 256;
    float range[] = { 0, 256 };
    const float* histRange = { range };
    bool uniform = true; bool accumulate = false;

    cv::Mat b_hist, g_hist, r_hist;
    cv::calcHist(&bgr_planes[0], 1, 0, cv::Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);
    cv::calcHist(&bgr_planes[1], 1, 0, cv::Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
    cv::calcHist(&bgr_planes[2], 1, 0, cv::Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);

    std::vector<cv::Mat> BGR_hist;
    BGR_hist.push_back(b_hist);
    BGR_hist.push_back(g_hist);
    BGR_hist.push_back(r_hist);
    return BGR_hist;
}

img::Image img::Histogram::createHistogramDisplayImage(std::vector<cv::Mat> bgrhist, int hist_w, int hist_h){
    int histSize = 256;
    int bin_w = cvRound((double)hist_w / histSize);

    cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(0, 0, 0));

    cv::Mat b_hist = normalizeMat(bgrhist[0], 0, histImage.rows);
    cv::Mat g_hist = normalizeMat(bgrhist[1], 0, histImage.rows);
    cv::Mat r_hist = normalizeMat(bgrhist[2], 0, histImage.rows);
    
    nhistBGR.push_back(b_hist);
    nhistBGR.push_back(g_hist);
    nhistBGR.push_back(r_hist);

    for (int i = 1; i < histSize; i++){
        line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
            cv::Point(bin_w * (i), hist_h - cvRound(b_hist.at<float>(i))),
            cv::Scalar(255, 0, 0), 2, 8, 0);
        line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))),
            cv::Point(bin_w * (i), hist_h - cvRound(g_hist.at<float>(i))),
            cv::Scalar(0, 255, 0), 2, 8, 0);
        line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(r_hist.at<float>(i - 1))),
            cv::Point(bin_w * (i), hist_h - cvRound(r_hist.at<float>(i))),
            cv::Scalar(0, 0, 255), 2, 8, 0);
    }
    Image imoperator(histImage);
    return imoperator;
}

img::Edge::Edge(img::Image* srimg, int flag) {
    srcimg = srimg;
    
}



img::Image* img::Edge::getSourceImg() {
    return srcimg;
}


std::vector<img::Image> img::readImageFolder(std::string imagefolderdirectory, int flag, bool all, int number) {
    if (imagefolderdirectory.find(".") != std::string::npos)
        throw std::exception("Illegal folder path.");
    else if (flag != cv::IMREAD_UNCHANGED && flag != cv::IMREAD_COLOR && flag != cv::IMREAD_GRAYSCALE)
        throw std::exception("Illegal image read flag.2");
    img::Image imgoperator;
    std::string pathstring;
    std::vector<img::Image> vecoperator;
    int iter = 0;
    for (const auto& entry : std::filesystem::directory_iterator(imagefolderdirectory)) {
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