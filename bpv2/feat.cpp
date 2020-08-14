#include "feat.h"
feat::Histogram::Histogram(img::Image* srimg, int fb, int sb, int tb, int flag) {
    srcimg = srimg;
    srcmat = srcimg->getImageMat();
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

feat::Histogram::Histogram(cv::Mat mat, int fb, int sb, int tb, int flag) {
    srcmat = mat;
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

cv::Mat feat::Histogram::getHistogramMat() {
    return histmat;
}

cv::Mat feat::Histogram::getNormalizedHistogramMat() {
    return nhistmat;
}

cv::Mat feat::Histogram::getSourceMat() {
    return srcmat;
}

int* feat::Histogram::getBin() {
    int binval[] = { fbin, sbin , tbin };
    return binval;
}

void feat::Histogram::setHistogramDisplayImage(int width, int height) {
    histBGR = histogramBGRSeparateCalculation(getSourceMat());
    *histdspimg = createHistogramDisplayImage(histBGR, width, height);
}

cv::Mat feat::Histogram::histogramHSVCalculation(cv::Mat sourcemat) {
    cv::MatND hsvbase, histbase;
    cv::cvtColor(sourcemat, hsvbase, cv::COLOR_BGR2HSV);
    int histSize[] = { getBin()[0], getBin()[1], getBin()[2] };

    float h_ranges[] = { 0, 180 };
    float sv_ranges[] = { 0, 256 };
    const float* histRange[] = { h_ranges, sv_ranges, sv_ranges };

    bool uniform = true; bool accumulate = false;
    int channels[] = { 0, 1, 2 };

    cv::calcHist(&hsvbase, 1, channels, cv::Mat(), histbase, 3, histSize, histRange, uniform, accumulate);
    return histbase;
}

cv::Mat feat::Histogram::histogramBGRCalculation(cv::Mat sourcemat) {
    int histSize[] = { getBin()[0], getBin()[1], getBin()[2] };

    float bgr_ranges[] = { 0, 256 };
    const float* histRange[] = { bgr_ranges, bgr_ranges , bgr_ranges };

    bool uniform = true; bool accumulate = false;
    int channels[] = { 0, 1, 2 };

    cv::MatND histbase;
    cv::calcHist(&sourcemat, 1, channels, cv::Mat(), histbase, 3, histSize, histRange, uniform, accumulate);
    return histbase;
}

cv::Mat feat::Histogram::histogramGRAYCalculation(cv::Mat sourcemat) {
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

cv::Mat feat::Histogram::normalizeMat(cv::Mat sourcemat, float alpha, float beta) {
    cv::Mat nhist;
    normalize(sourcemat, nhist, alpha, beta, cv::NORM_MINMAX, -1, cv::Mat());
    return nhist;
}

std::vector<cv::Mat> feat::Histogram::histogramBGRSeparateCalculation(cv::Mat sourcemat) {
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

img::Image feat::Histogram::createHistogramDisplayImage(std::vector<cv::Mat> bgrhist, int hist_w, int hist_h) {
    int histSize = 256;
    int bin_w = cvRound((double)hist_w / histSize);

    cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(0, 0, 0));

    cv::Mat b_hist = normalizeMat(bgrhist[0], 0, histImage.rows);
    cv::Mat g_hist = normalizeMat(bgrhist[1], 0, histImage.rows);
    cv::Mat r_hist = normalizeMat(bgrhist[2], 0, histImage.rows);

    nhistBGR.push_back(b_hist);
    nhistBGR.push_back(g_hist);
    nhistBGR.push_back(r_hist);

    for (int i = 1; i < histSize; i++) {
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
    img::Image imoperator(histImage);
    return imoperator;
}

feat::Edge::Edge(img::Image* srimg, int flag) {
    srcimg = srimg;

}

img::Image* feat::Edge::getSourceImg() {
    return srcimg;
}