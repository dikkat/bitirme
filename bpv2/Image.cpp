#include "Image.h"

img::Image::Image(std::string imgdir, int flag) {
	imagemat = readImageFile(imgdir, flag);
    Histogram xd(imagemat);
}

cv::Mat img::Image::readImageFile(std::string imagedirectory, int flag) {
	if (flag != cv::IMREAD_UNCHANGED && flag != cv::IMREAD_COLOR && flag != cv::IMREAD_GRAYSCALE)
		throw std::exception("Illegal image read flag.1");
	cv::Mat matoperator;
	matoperator = cv::imread(imagedirectory, flag);
	if (matoperator.data == NULL)
		throw std::exception("Can't read the file.");
	return matoperator;
}

std::vector<cv::Mat> img::Image::readImageFolder(std::string imagefolderdirectory, int flag) {
	if (imagefolderdirectory.find(".") != std::string::npos)
		throw std::exception("Illegal folder path.");
	else if (flag != cv::IMREAD_UNCHANGED && flag != cv::IMREAD_COLOR && flag != cv::IMREAD_GRAYSCALE)
		throw std::exception("Illegal image read flag.2");
	cv::Mat matoperator;
	std::string pathstring;
	std::vector<cv::Mat> vecoperator;
	for (const auto& entry : std::filesystem::directory_iterator(imagefolderdirectory)) {
		pathstring = entry.path().u8string();
		matoperator = cv::imread(pathstring, flag);
		if (matoperator.data == NULL) {
			continue;
		}
		vecoperator.push_back(matoperator);
	}
	return vecoperator;
}

img::Histogram::Histogram(cv::Mat sourcemat) {
    histmat = histogramCalculation(sourcemat);
    nhistmat = histogramNormalizedCalculation(histmat);
    histBGR = histogramBGRCalculation(sourcemat);
}

cv::Mat img::Histogram::histogramCalculation(cv::Mat sourcemat) {
    int histSize = 256;
    float range[] = { 0, 256 };
    const float* histRange = { range };
    bool uniform = true; bool accumulate = false;
    int channels[] = { 0, 1 };
    cv::Mat hist;
    cv::calcHist(&sourcemat, 1, channels, cv::Mat(), hist, 1, &histSize, &histRange, uniform, accumulate);
    return hist;
}

cv::Mat img::Histogram::histogramNormalizedCalculation(cv::Mat sourcemat) {
    cv::Mat nhist;
    normalize(histmat, nhist, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
    return nhist;
}

std::vector<cv::Mat> img::Histogram::histogramBGRCalculation(cv::Mat sourcemat) {
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


    int hist_w = 512; int hist_h = 400;
    int bin_w = cvRound((double)hist_w / histSize);

    cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(0, 0, 0));

    normalize(b_hist, b_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat()); //FIND OUT THE DIFFERENCE BETWEEN HISTIMAGE.ROWS=400 AND 1
    normalize(g_hist, g_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());
    normalize(r_hist, r_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());

    for (int i = 1; i < histSize; i++)
    {
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
    return bgr_planes;
}



