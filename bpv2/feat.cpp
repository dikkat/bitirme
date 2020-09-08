#include "feat.h"

cv::Mat feat::sobelX = (cv::Mat_<float>(3, 3) << 1, 0, -1, 2, 0, -2, 1, 0, -1);
cv::Mat feat::sobelY = (cv::Mat_<float>(3, 3) << 1, 2, 1, 0, 0, 0, -1, -2, -1);
cv::Mat feat::prewittX = (cv::Mat_<float>(3, 3) << 1, 0, -1, 1, 0, -1, 1, 0, -1);
cv::Mat feat::prewittY = (cv::Mat_<float>(3, 3) << 1, 1, 1, 0, 0, 0, -1, -1, -1);
cv::Mat feat::robertX = (cv::Mat_<float>(2, 2) << 1, 0, 0, -1);
cv::Mat feat::robertY = (cv::Mat_<float>(2, 2) << 0, 1, -1, 0);

feat::Histogram::Histogram(cv::Mat imageMat, int fb, int sb, int tb, int flag) {
    fbin = fb;
    sbin = sb;
    tbin = tb;

    if (flag == -1) {
        if (imageMat.channels() == 1)
            histMat = histogramGRAYCalculation(imageMat);
        else if (imageMat.channels() == 3)
            histMat = histogramBGRCalculation(imageMat);
        else
            throw std::exception("Illegal histogram build flag.");
    }

    else if (flag == HIST_BGR)
        histMat = histogramBGRCalculation(imageMat);
    else if (flag == HIST_HSV)
        histMat = histogramHSVCalculation(imageMat);
    else if (flag == HIST_GRAY)
        histMat = histogramGRAYCalculation(imageMat);
    else
        throw std::exception("Illegal histogram build flag.");

    nhistMat = normalizeMat(histMat, 0, 1);
    histFlag = flag;
    sourceMat = imageMat;
    histHash = feat::Hash::setHash(nullptr, &std::vector<float>{static_cast<float>(flag), static_cast<float>(fb),
        static_cast<float>(sb), static_cast<float>(tb)});
}

cv::Mat feat::Histogram::getHistogramMat() {
    return histMat;
}

cv::Mat feat::Histogram::getNormalizedHistogramMat() {
    return nhistMat;
}

int* feat::Histogram::getBin() {
    int binval[] = { fbin, sbin , tbin };
    return binval;
}

std::vector<float> feat::Histogram::getVariablesFloat() {
    return std::vector<float>{static_cast<float>(histFlag), static_cast<float>(fbin),
        static_cast<float>(sbin), static_cast<float>(tbin)};
}

XXH64_hash_t feat::Histogram::getHash() {
    return histHash;
}

cv::Mat feat::Histogram::histogramHSVCalculation(cv::Mat sourceMat) {
    cv::MatND hsvbase, histbase;
    cv::cvtColor(sourceMat, hsvbase, cv::COLOR_BGR2HSV);
    int histSize[] = { getBin()[0], getBin()[1], getBin()[2] };

    float h_ranges[] = { 0, 180 };
    float sv_ranges[] = { 0, 256 };
    const float* histRange[] = { h_ranges, sv_ranges, sv_ranges };

    bool uniform = true; bool accumulate = false;
    int channels[] = { 0, 1, 2 };

    cv::calcHist(&hsvbase, 1, channels, cv::Mat(), histbase, 3, histSize, histRange, uniform, accumulate);
    return histbase;
}

cv::Mat feat::Histogram::histogramBGRCalculation(cv::Mat sourceMat) {
    int histSize[] = { getBin()[0], getBin()[1], getBin()[2] };

    float bgr_ranges[] = { 0, 256 };
    const float* histRange[] = { bgr_ranges, bgr_ranges , bgr_ranges };

    bool uniform = true; bool accumulate = false;
    int channels[] = { 0, 1, 2 };

    cv::MatND histbase;
    cv::calcHist(&sourceMat, 1, channels, cv::Mat(), histbase, 3, histSize, histRange, uniform, accumulate);
    return histbase;
}

cv::Mat feat::Histogram::histogramGRAYCalculation(cv::Mat sourceMat) {
    int histSize[] = { getBin()[0] };

    cv::Mat greyMat;
    if (sourceMat.channels() == 3)
        cv::cvtColor(sourceMat, greyMat, cv::COLOR_BGR2GRAY);

    float gray_range[] = { - pow(2,32) + 1, pow(2,32) - 1 };
    const float* histRange[] = { gray_range };

    bool uniform = true; bool accumulate = false;
    int channels[] = { 0 };

    cv::MatND histbase;
    cv::calcHist(&sourceMat, 1, channels, cv::Mat(), histbase, 1, histSize, histRange, uniform, accumulate);
    return histbase;
}

cv::Mat feat::Histogram::normalizeMat(cv::Mat sourceMat, float alpha, float beta) {
    cv::Mat nhist;
    normalize(sourceMat, nhist, alpha, beta, cv::NORM_MINMAX, -1, cv::Mat());
    return nhist;
}

std::vector<cv::Mat> feat::Histogram::histogramBGRSeparateCalculation(cv::Mat sourceMat) {
    std::vector<cv::Mat> bgr_planes;
    cv::split(sourceMat, bgr_planes);

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

cv::Mat feat::Histogram::createHistogramDisplayImage(std::vector<cv::Mat> bgrhist, int hist_w, int hist_h) {
    int histSize = 256;
    int bin_w = cvRound((double)hist_w / histSize);

    cv::Mat histImgMat(hist_h, hist_w, CV_8UC3, cv::Scalar(0, 0, 0));

    cv::Mat b_hist = normalizeMat(bgrhist[0], 0, histImgMat.rows);
    cv::Mat g_hist = normalizeMat(bgrhist[1], 0, histImgMat.rows);
    cv::Mat r_hist = normalizeMat(bgrhist[2], 0, histImgMat.rows);

    nhistBGR.push_back(b_hist);
    nhistBGR.push_back(g_hist);
    nhistBGR.push_back(r_hist);

    for (int i = 1; i < histSize; i++) {
        line(histImgMat, cv::Point(bin_w * (i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
            cv::Point(bin_w * (i), hist_h - cvRound(b_hist.at<float>(i))),
            cv::Scalar(255, 0, 0), 2, 8, 0);
        line(histImgMat, cv::Point(bin_w * (i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))),
            cv::Point(bin_w * (i), hist_h - cvRound(g_hist.at<float>(i))),
            cv::Scalar(0, 255, 0), 2, 8, 0);
        line(histImgMat, cv::Point(bin_w * (i - 1), hist_h - cvRound(r_hist.at<float>(i - 1))),
            cv::Point(bin_w * (i), hist_h - cvRound(r_hist.at<float>(i))),
            cv::Scalar(0, 0, 255), 2, 8, 0);
    }
    return histImgMat;
}

feat::Edge::Edge(cv::Mat imageMat, int flag, feat::Edge::Canny* edc) {
    cv::Mat oper;
    switch (flag) {
    case EDGE_SOBEL:
        oper = edgeDetectionSobel(imageMat);
        break;
    case EDGE_PREWT:
        oper = edgeDetectionPrewitt(imageMat);
        break;
    case EDGE_ROBRT:
        oper = edgeDetectionRobertsCross(imageMat);
        break;
    case EDGE_CANNY:
        if (edc == nullptr)
            throw std::exception("Initialize Canny class first.");
        else {
            child = edc;
            edc->parent = this;
            oper = edc->edgeDetectionCanny(imageMat);
        }
        break;
    default:
        throw std::exception("Illegal edge detection flag.");
    }
    edgeFlag = flag;
    sourceMat = oper;
}

int feat::Edge::getEdgeFlag() { 
    return edgeFlag; 
}

std::vector<XXH64_hash_t> feat::Edge::getHashVariables() {
    if(edcHash != nullptr)
        return std::vector<XXH64_hash_t>{edgeHash};
    else
        return std::vector<XXH64_hash_t>{edgeHash, *edcHash};
}

feat::Edge::Canny* feat::Edge::getCannyPtr() {
    return child;
}

cv::Mat feat::Edge::edgeDetectionSobel(cv::Mat const imageMat) {
    cv::Mat sobelg = commonOperationsSPR(sobelX, sobelY, imageMat);
    return sobelg;
}

cv::Mat feat::Edge::edgeDetectionPrewitt(cv::Mat const imageMat) {
    cv::Mat prewittg = commonOperationsSPR(prewittX, prewittY, imageMat);
    return prewittg;
}

cv::Mat feat::Edge::edgeDetectionRobertsCross(cv::Mat const imageMat) {
    cv::Mat robertsg = commonOperationsSPR(robertX, robertY, imageMat);
    return robertsg;
}

cv::Mat feat::Edge::edgeDetectionDeriche(cv::Mat const imageMat, float alpha) {
    float k = (pow(1 - pow(M_E, -alpha), 2)) / (1 + 2 * alpha * pow(M_E, -alpha) - pow(M_E, -2 * alpha));
    float as[9], ax[9], ay[9], b[3], cs[3], cx[3], cy[3];

    //https://en.wikipedia.org/wiki/Deriche_edge_detector
    as[1] = k, as[2] = k * pow(M_E, -alpha) * (alpha - 1), as[3] = k * pow(M_E, -alpha) * (alpha + 1), as[4] = -k * pow(M_E, -2 * alpha);
    ax[1] = 0, ax[2] = 1, ax[3] = -1, ax[4] = 0;
    for (int i = 1; i < 5; i++) {
        as[i + 4] = as[i];
        ax[i + 4] = as[i];
        ay[i] = as[i];
        ay[i + 4] = ax[i];
    }

    b[1] = 2 * pow(M_E, -alpha);
    b[2] = pow(-M_E, -2 * alpha);

    cs[1] = cs[2] = 1;
    cx[1] = -(pow(1 - pow(M_E, -alpha), 2)), cx[2] = 1;
    cy[1] = 1, cy[2] = cx[1];

    
    auto operationBody = [&b](cv::Mat imageMat, float a[], float c[]) {
        cv::Mat bufferMat1(imageMat.rows, imageMat.cols, CV_32FC1);
        cv::Mat bufferMat2 = bufferMat1.clone();
        cv::Mat oper;
        if (imageMat.channels() == 3) {
            cv::cvtColor(imageMat, oper, cv::COLOR_BGR2GRAY);
            oper.convertTo(oper, CV_32FC1);
        }
        else
            oper = imageMat.clone();
        for (int i = 0; i < oper.rows; i++)
            for (int j = 0; j < oper.cols; j++) {
                if (j - 1 < 0)
                    bufferMat1.at<float>(i, j) = a[1] * oper.at<float>(i, j);
                else if (j - 2 < 0)
                    bufferMat1.at<float>(i, j) = a[1] * oper.at<float>(i, j) + a[2] * oper.at<float>(i, j - 1)
                    + b[1] * bufferMat1.at<float>(i, j - 1);
                else
                    bufferMat1.at<float>(i, j) = a[1] * oper.at<float>(i, j) + a[2] * oper.at<float>(i, j - 1)
                    + b[1] * bufferMat1.at<float>(i, j - 1) + b[2] * bufferMat1.at<float>(i, j - 2);
            }

        for (int i = 0; i < oper.rows; i++)
            for (int j = oper.cols - 1; j > -1; j--) {
                if (j + 1 > oper.cols - 1)
                    bufferMat2.at<float>(i, j) = 0;
                else if (j + 2 > oper.cols - 1)
                    bufferMat2.at<float>(i, j) = a[3] * oper.at<float>(i, j + 1)
                    + b[1] * bufferMat2.at<float>(i, j + 1);
                else
                    bufferMat2.at<float>(i, j) = a[3] * oper.at<float>(i, j + 1) + a[4] * oper.at<float>(i, j + 2)
                    + b[1] * bufferMat2.at<float>(i, j + 1) + b[2] * bufferMat2.at<float>(i, j + 2);
            }

        cv::Mat tempMat(oper.rows, oper.cols, CV_32FC1);
        tempMat = c[1] * (bufferMat1 + bufferMat2);

        for (int i = 0; i < oper.rows; i++)
            for (int j = 0; j < oper.cols; j++) {
                if (i - 1 < 0)
                    bufferMat1.at<float>(i, j) = a[5] * tempMat.at<float>(i, j);
                else if (i - 2 < 0)
                    bufferMat1.at<float>(i, j) = a[5] * tempMat.at<float>(i, j) + a[6] * tempMat.at<float>(i - 1, j)
                    + b[1] * bufferMat1.at<float>(i - 1, j);
                else
                    bufferMat1.at<float>(i, j) = a[5] * tempMat.at<float>(i, j) + a[6] * tempMat.at<float>(i - 1, j)
                    + b[1] * bufferMat1.at<float>(i - 1, j) + b[2] * bufferMat1.at<float>(i - 2, j);
            }

        for (int i = oper.rows - 1; i > -1; i--)
            for (int j = 0; j < oper.cols; j++) {
                if (i + 1 > oper.rows - 1)
                    bufferMat2.at<float>(i, j) = 0;
                else if (i + 2 > oper.rows - 1)
                    bufferMat2.at<float>(i, j) = a[7] * tempMat.at<float>(i + 1, j)
                    + b[1] * bufferMat2.at<float>(i + 1, j);
                else
                    bufferMat2.at<float>(i, j) = a[3] * tempMat.at<float>(i + 1, j) + a[4] * tempMat.at<float>(i + 2, j)
                    + b[1] * bufferMat2.at<float>(i + 1, j) + b[2] * bufferMat2.at<float>(i + 2, j);
            }

        cv::Mat resultMat(oper.rows, oper.cols, CV_32FC1);
        resultMat = c[2] * (bufferMat1 + bufferMat2);
        return resultMat;
    };

    cv::Mat smoothMat = operationBody(imageMat, as, cs);
    cv::Mat xMat = operationBody(smoothMat, ax, cx);
    cv::Mat yMat = operationBody(smoothMat, ay, cy);
    
    cv::Mat resultMat(imageMat.rows, imageMat.cols, CV_32FC1);
    
    for (int i = 0; i < resultMat.rows; i++) {
        for (int j = 0; j < resultMat.cols; j++)
            resultMat.at<float>(i, j) = hypot(xMat.at<float>(i, j), yMat.at<float>(i, j));
    }

    cv::normalize(resultMat, resultMat, 0, 255, cv::NORM_MINMAX);
   
    return resultMat;
}


cv::Mat feat::Edge::commonOperationsSPR(cv::Mat const kx, cv::Mat const ky, cv::Mat const imat) {
    cv::Mat kernelx = kx;
    cv::Mat kernely = ky;

    cv::Mat sprgx = sim::convolution2DopenCV(imat, kernelx);
    cv::Mat sprgy = sim::convolution2DopenCV(imat, kernely);

    cv::Mat sprg(sprgx.dims, sprgx.size, sprgx.type());
    for (int i = 0; i < sprg.rows; i++) {
        for (int j = 0; j < sprg.cols; j++)
            sprg.at<float>(i, j) = hypot(sprgx.at<float>(i, j), sprgy.at<float>(i, j));
    }

    sprg(cv::Range(floor(kx.rows / 2), sprg.rows - floor(kx.rows / 2)), cv::Range(floor(kx.cols / 2), sprg.cols - floor(kx.cols / 2))).copyTo(sprg);

    cv::normalize(sprg, sprg, 0, 255, cv::NORM_MINMAX);
    return sprg;
}

std::vector<cv::Mat> feat::Edge::calculateEdgeGradientMagnitudeDirection(cv::Mat const kx, cv::Mat const ky, cv::Mat const imat) {
    cv::Mat kernelx = kx;
    cv::Mat kernely = ky;

    cv::Mat sprgx = sim::convolution2D(imat, kernelx);
    cv::Mat sprgy = sim::convolution2D(imat, kernely);

    cv::Mat sprgMag(sprgx.dims, sprgx.size, sprgx.type());
    cv::Mat sprgDir(sprgx.dims, sprgx.size, sprgx.type());

    for (int i = 0; i < sprgMag.rows; i++) {
        for (int j = 0; j < sprgMag.cols; j++) {
            sprgMag.at<float>(i, j) = hypot(sprgx.at<float>(i, j), sprgy.at<float>(i, j));
        }
    }

    double sprgMAX;
    cv::minMaxLoc(sprgMag, 0, &sprgMAX);
    sprgMag = sprgMag / sprgMAX * 255;

    for (int i = 0; i < sprgDir.rows; i++) {
        for (int j = 0; j < sprgDir.cols; j++) {
            sprgDir.at<float>(i, j) = atan2f(sprgy.at<float>(i, j), sprgx.at<float>(i, j));
            if (sprgDir.at<float>(i, j) < 0)
                sprgDir.at<float>(i, j) += M_PI * 2;
        }
    }

    return std::vector<cv::Mat>{sprgMag, sprgDir};
}

cv::Mat feat::Edge::Canny::edgeDetectionCanny(cv::Mat const imageMat) {
    if (sourceMat.data == NULL) {
        parent->edcHash = &Hash;
        sourceMat = calculate(imageMat);
        return sourceMat;
    }
    else
        return sourceMat;
}

////kernelx, kernely, gaussKernelSize, thigh, tlow, sigma
//void feat::Edge::Canny::setVariables(std::string varName, float fltVal, cv::Mat matVal) {
//    if (varName == "kernelx")
//        kernelx = matVal;
//    else if (varName == "kernely")
//        kernely = matVal;
//    else if (varName == "gaussKernelSize")
//        gaussKernelSize = fltVal;
//    else if (varName == "thigh")
//        thigh = fltVal;
//    else if (varName == "tlow")
//        tlow = fltVal;
//    else if (varName == "sigma")
//        sigma = fltVal;
//    else
//        throw std::exception("Illegal variable flag.");
//    setHash();
//}

std::vector<float> feat::Edge::Canny::getVariablesFloat() {
    return std::vector<float>{gaussKernelSize, thigh, tlow, sigma};
}

std::vector<cv::Mat> feat::Edge::Canny::getVariablesMat() {
    return std::vector<cv::Mat>{kernelx, kernely};
}

XXH64_hash_t feat::Edge::Canny::getHash() {
    return Hash;
}

void feat::Edge::Canny::setHash() {
    Hash = feat::Hash::setHash(&getVariablesMat(), &getVariablesFloat());
}

cv::Mat feat::Edge::Canny::calculate(cv::Mat const imageMat) { //Computer Vision, Mar 2000, Alg 24
    //http://www.tomgibara.com/computer-vision/CannyEdgeDetector
    cv::Mat cannyoper = sim::channelCheck(imageMat);																							 //https://towardsdatascience.com/canny-edge-detection-step-by-step-in-python-computer-vision-b49c3a2d8123

    cannyoper = sim::filterGauss(cannyoper, gaussKernelSize, sigma);
    cv::Mat kernelx = (cv::Mat_<float>(3, 3) << -1, 0, 1, -2, 0, 2, -1, 0, 1);
    cv::Mat kernely = (cv::Mat_<float>(3, 3) << 1, 2, 1, 0, 0, 0, -1, -2, -1);
    std::vector<cv::Mat> temp = feat::Edge::calculateEdgeGradientMagnitudeDirection(kernelx, kernely, cannyoper);
    cv::Mat magMat = std::ref(temp[0]);
    cv::Mat dirMat = std::ref(temp[1]);

    cv::Mat nonMaximaMat = nonMaximumSuppression(dirMat, magMat);

    double max;
    cv::minMaxLoc(nonMaximaMat, 0, &max);

    float hThreshold = max * thigh;
    float lThreshold = hThreshold * tlow;

    float weakratio = 0.09; //has no effect
    cv::Mat dtMat(nonMaximaMat.rows, nonMaximaMat.cols, nonMaximaMat.type());
    doubleThreshold(dtMat, nonMaximaMat, max, lThreshold, hThreshold, weakratio);

    float strong = max;
    float weak = strong * weakratio;

    performHysteresis(dtMat, weak, strong);
    return dtMat;
}


cv::Mat feat::Edge::Canny::nonMaximumSuppression(cv::Mat& dirMat, cv::Mat& magMat) {
    cv::Mat resultMat = cv::Mat::zeros(magMat.rows, magMat.cols, magMat.type());
    cv::Mat angleMat = cv::Mat::zeros(dirMat.rows, dirMat.cols, dirMat.type());

    for (int i = 0; i < angleMat.total(); i++) {
        angleMat.at<float>(i) = dirMat.at<float>(i) * 180 / M_PI;
        if (angleMat.at<float>(i) < 0)
            angleMat.at<float>(i) += 2 * M_PI;
    }

    for (int i = 0; i < magMat.rows; i++) {
        for (int j = 0; j < magMat.cols; j++) {
            float q = 255;
            float r = 255;
            float angleOper = angleMat.at<float>(i, j);

            if ((0 <= angleOper && angleOper < 22.5) || (337.5 <= angleOper && angleOper < 360) || (157.5 <= angleOper && angleOper < 202.5)) {
                if (j - 1 < 0 || j + 1 >= magMat.cols)
                    continue;
                q = magMat.at<float>(i, j + 1);
                r = magMat.at<float>(i, j - 1);
            }

            else if ((22.5 <= angleOper && angleOper < 67.5) || 202.5 <= angleOper && angleOper < 247.5) {
                if (i - 1 < 0 || j - 1 < 0 || i + 1 >= magMat.rows || j + 1 >= magMat.cols)
                    continue;
                q = magMat.at<float>(i + 1, j - 1);
                r = magMat.at<float>(i - 1, j + 1);
            }

            else if ((67.5 <= angleOper && angleOper < 112.5) || (247.5 <= angleOper && angleOper < 292.5)) {
                if (i - 1 < 0 || i + 1 >= magMat.rows)
                    continue;
                q = magMat.at<float>(i + 1, j);
                r = magMat.at<float>(i - 1, j);
            }

            else if ((112.5 <= angleOper && angleOper < 157.5) || (292.5 <= angleOper && angleOper < 337.5)) {
                if (i - 1 < 0 || j - 1 < 0 || i + 1 >= magMat.rows || j + 1 >= magMat.cols)
                    continue;
                q = magMat.at<float>(i - 1, j - 1);
                r = magMat.at<float>(i + 1, j + 1);
            }

            if (magMat.at<float>(i, j) >= q && magMat.at<float>(i, j) >= r)
                resultMat.at<float>(i, j) = magMat.at<float>(i, j);
            else
                resultMat.at<float>(i, j) = 0;
        }
    }
    return resultMat;
}

void feat::Edge::Canny::doubleThreshold(cv::Mat& resultMat, cv::Mat const nonMaximaMat, float max, float lThreshold, float hThreshold, float weakratio) { //https://towardsdatascience.com/canny-edge-detection-step-by-step-in-python-computer-vision-b49c3a2d8123
    float strong = max;
    float weak = strong * weakratio;


    for (int i = 0; i < nonMaximaMat.total(); i++) {
        if (nonMaximaMat.at<float>(i) > hThreshold)
            resultMat.at<float>(i) = strong;
        else if (nonMaximaMat.at<float>(i) < lThreshold)
            resultMat.at<float>(i) = 0;
        else
            resultMat.at<float>(i) = weak;
    }
}

void feat::Edge::Canny::performHysteresis(cv::Mat& resultMat, float weak, float strong) {
    for (int i = 0; i < resultMat.rows; i++) {
        for (int j = 0; j < resultMat.cols; j++) {
            if (resultMat.at<float>(i, j) == weak) {
                if (i - 1 < 0 || i + 1 >= resultMat.rows || j - 1 < 0 || j + 1 >= resultMat.cols) {
                    resultMat.at<float>(i, j) = 0;
                    continue;
                }
                if ((resultMat.at<float>(i - 1, j - 1) == strong)
                    || (resultMat.at<float>(i - 1, j) == strong)
                    || (resultMat.at<float>(i - 1, j + 1) == strong)
                    || (resultMat.at<float>(i, j - 1) == strong)
                    || (resultMat.at<float>(i, j + 1) == strong)
                    || (resultMat.at<float>(i + 1, j - 1) == strong)
                    || (resultMat.at<float>(i + 1, j) == strong)
                    || (resultMat.at<float>(i + 1, j + 1) == strong))
                    resultMat.at<float>(i, j) = strong;
                else
                    resultMat.at<float>(i, j) = 0;
            }
        }
    }
}

feat::Corner::Corner(cv::Mat imageMat, int flag, int numofScales, float scaleRat, feat::Corner::Harris* cdh) {
    cv::Mat oper;
    switch (flag) {
    case CORNER_HARLAP:
        oper = cornerDetectionHarrisLaplace(imageMat, numofScales, scaleRat);
        break;
    case CORNER_HARRIS:
        if (cdh == nullptr)
            throw std::exception("Initialize Harris class first.");
        else {
            child = cdh;
            cdh->parent = this;
            oper = cdh->cornerDetectionHarris(imageMat);
        }
        break;
    default:
        throw std::exception("Illegal edge detection flag.");
    }
    edgeFlag = flag;
    sourceMat = oper;
}

cv::Mat feat::Corner::paintPointsOverImage(cv::Mat const imageMat, cv::Mat const pointMat, bool gray, float numOfPoints, float radius, float thickness, cv::Scalar pointColor) {
    std::vector<cv::Point> pointVec;

    cv::Mat pointMatOper = pointMat.clone();

    for (int i = 0; i < numOfPoints; i++) {
        cv::Point pointOper;
        double value;
        cv::minMaxLoc(pointMatOper, NULL, &value, NULL, &pointOper);
        if (value == 0)
            break;
        pointVec.push_back(pointOper);
        pointMatOper.at<float>(pointOper) = 0;
    }
    cv::Mat imgOper = imageMat.clone();
    if(imgOper.channels() == 1)
        cv::cvtColor(imgOper, imgOper, cv::COLOR_GRAY2BGR);

    for (int i = 0; i < pointVec.size(); i++)
        cv::circle(imgOper, pointVec[i], radius, pointColor, thickness);

    return imgOper;
}

cv::Mat feat::Corner::cornerDetectionHarrisLaplace(cv::Mat imageMat, float n, float scaleRatio) {
    //https://www.robots.ox.ac.uk/~vgg/research/affine/det_eval_files/mikolajczyk_ijcv2004.pdf
    //%85 ben -- %15 https://www.mathworks.com/matlabcentral/fileexchange/64689-harris-affine-and-harris-laplace-interest-point-detector
    //VEEEEEEEEEEERY SLOW
    int recommendedWidth = 375, width, height;
    float scale;

    if (scaleRatio == 0) {
        if (imageMat.cols > recommendedWidth)
            scale = recommendedWidth / imageMat.cols;
        else
            scale = 1;
        width = imageMat.cols * scale;
        height = imageMat.rows * scale;
    }

    else if (scaleRatio < 0)
        throw std::exception("Illegal scale ratio.");

    else {
        width = imageMat.cols * scaleRatio;
        height = imageMat.rows * scaleRatio;
    }

    cv::Mat imgOper = sim::channelCheck(imageMat);

    cv::resize(imgOper, imgOper, cv::Size(width, height));
    std::vector<float> setOfScales;
    float ki = 1.4;
    float sigma0 = 1.0;
    for (int i = 1; i < n + 1; i++)
        setOfScales.push_back(pow(ki, i) * sigma0);

    float scaleConstant = 0.7;

    std::vector<cv::Mat> setOfDerivatives;

    std::vector<std::vector<cv::Point>> pointLocVec;
    pointLocVec.resize(n);

    //POINT CALCULATION FOR EVERY SCALE
    for (int i = 0; i < n; i++) {
        float gauss_size = 31;

        cv::Mat LoGDerivative = sim::filterGauss(imgOper, gauss_size, setOfScales[i], 1, true);
        cv::Laplacian(LoGDerivative, LoGDerivative, CV_32F, 5);
        setOfDerivatives.push_back(LoGDerivative);

        Harris cdhOper;
        cdhOper.setVariables("sigmai", setOfScales[i]);
        cdhOper.setVariables("sigmad", scaleConstant * setOfScales[i]);
        cdhOper.setVariables("squareSize", 5);
        
        cv::Mat harrisPointMat = cdhOper.cornerDetectionHarris(imgOper);

        for (int j = 0; j < harrisPointMat.rows; j++) {
            for (int k = 0; k < harrisPointMat.cols; k++) {
                if (harrisPointMat.at<float>(j, k) > 1) {
                    cv::Point pointOper(k, j);
                    pointLocVec[i].push_back(pointOper);
                }
            }
        }

        /*cv::Mat imgoperx = imgOper.clone();

        imgoperx = sim::filterGauss(imgoperx, 7, setOfScales[i] * scaleConstant, 1, true);
        cv::normalize(imgoperx, imgoperx, 0, 255, cv::NORM_MINMAX);

        for (int j = 0; j < pointLocVec[i].size(); j++)
            cv::circle(imgoperx, pointLocVec[i][j], 1, { 0,0,0 }, 1);
        gen::imageTesting(imgoperx, "tester" + std::to_string(i) + "3");*/
    }    

    cv::Mat resultMat = cv::Mat::zeros(imageMat.rows, imageMat.cols, CV_32FC1);

    //LOG CALCULATION FOR EVERY POINT
    for (int i = 1; i < n - 1; i++) {
        for (int j = 0; j < pointLocVec[i].size(); j++) {
            float L = 0;
            float LoG[3] = { 0,0,0 };

            int x = pointLocVec[i][j].x;
            int y = pointLocVec[i][j].y;

            for (int k = -1; k < 2; k++) {
                L = 0;

                if (i + k < 0)
                    k++;

                else if (i + k >= n)
                    continue;

                for (int p = 0; p < 3; p++) {
                    if (x - 1 + p < 0 || x - 1 + p >= setOfDerivatives[i + k].cols)
                        continue;
                    for (int q = 0; q < 3; q++) {
                        if (y - 1 + q < 0 || y - 1 + q >= setOfDerivatives[i + k].rows)
                            continue;
                        L += setOfDerivatives[i + k].at<float>(y - 1 + q, x - 1 + p);
                    }
                }

                LoG[k + 1] = abs(pow(setOfScales[i + k], 2) * L);
            }

            if (LoG[1] > LoG[0] && LoG[1] > LoG[2]) {
                pointLocVec[i][j] *= 1 / scaleRatio;
                resultMat.at<float>(pointLocVec[i][j]) = 255;
            }

        }
    }
    return resultMat;
}

void feat::Corner::Harris::setVariables(std::string varName, float fltVal, cv::Mat* matVal) {
    if (varName == "kernelx")
        kernelx = *matVal;
    else if (varName == "kernely")
        kernely = *matVal;
    else if (varName == "radius")
        radius = fltVal;
    else if (varName == "squareSize")
        squareSize = fltVal;
    else if (varName == "sigmai")
        sigmai = fltVal;
    else if (varName == "sigmad")
        sigmad = fltVal;
    else if (varName == "alpha")
        alpha = fltVal;
    else
        throw std::exception("Illegal variable flag.");
    //setHash();
}

std::vector<float> feat::Corner::Harris::getVariablesFloat() {
    return std::vector<float>{radius, squareSize, sigmai, sigmad, alpha};
}

std::vector<cv::Mat> feat::Corner::Harris::getVariablesMat() {
    return std::vector<cv::Mat>{kernelx, kernely };
}

XXH64_hash_t feat::Corner::Harris::getVariableHash() {
    return hash;
}

cv::Mat feat::Corner::Harris::cornerDetectionHarris(cv::Mat const imageMat) {
    if (sourceMat.data == NULL) {
        parent->cdhHash = &hash;
        sourceMat = calculate(imageMat);
        return sourceMat;
    }
    else
        return sourceMat;
}

void feat::Corner::Harris::setHash() {
    hash = feat::Hash::setHash(&getVariablesMat(), &getVariablesFloat());
}

cv::Mat feat::Corner::Harris::calculate(cv::Mat const imageMat) {
    // %80 ben -- %20 https://www.mathworks.com/matlabcentral/fileexchange/64689-harris-affine-and-harris-laplace-interest-point-detector
    cv::Mat imgOper = sim::channelCheck(imageMat), resultOper = cv::Mat::zeros(imgOper.rows, imgOper.cols, CV_32FC1);

    float gauss_size = 31;
    imgOper = sim::filterGauss(imgOper, gauss_size, sigmad, 1, true);
    cv::normalize(imgOper, imgOper, 0, 255, cv::NORM_MINMAX);
    cv::Mat derivativeX = sim::convolution2D(imgOper, kernelx);
    cv::Mat derivativeY = sim::convolution2D(imgOper, kernely);

    cv::Mat derivativeXX = derivativeX.clone();
    for (int i = 0; i < derivativeX.total(); i++)
        derivativeXX.at<float>(i) = pow(derivativeX.at<float>(i), 2);
    derivativeXX = sim::filterGauss(derivativeXX, gauss_size, sigmai, 1, true);

    cv::Mat derivativeYY = derivativeY.clone();
    for (int i = 0; i < derivativeX.total(); i++)
        derivativeYY.at<float>(i) = pow(derivativeY.at<float>(i), 2);
    derivativeYY = sim::filterGauss(derivativeYY, gauss_size, sigmai, 1, true);
    
    cv::Mat derivativeXY = derivativeX.clone();
    for (int i = 0; i < derivativeXY.total(); i++)
        derivativeXY.at<float>(i) = derivativeX.at<float>(i) * derivativeY.at<float>(i);
    derivativeXY = sim::filterGauss(derivativeXY, gauss_size, sigmai, 1, true);
    
    float center = floor(radius / 2);

    for (int i = center; i < imgOper.rows - center - 1; i++)
        for (int j = center; j < imgOper.cols - center - 1; j++) {
            cv::Mat tensor = cv::Mat::zeros(2, 2, CV_32FC1);
            /*float weightF = pow(sigmad, 2);
            tensor.at<float>(0, 0) = weightF * derivativeXX.at<float>(i, j);
            tensor.at<float>(0, 1) = weightF * derivativeXY.at<float>(i, j);
            tensor.at<float>(1, 0) = weightF * derivativeXY.at<float>(i, j);
            tensor.at<float>(1, 1) = weightF * derivativeYY.at<float>(i, j);*/
            
            for (int p = 0; p < radius; p++)
                for (int q = 0; q < radius; q++) {
                    float weightF = 1 / (2 * M_PI * pow(sigmai, 2)) * pow(M_E, -((pow(p, 2) + pow(q, 2)) / 2 * pow(sigmai, 2)));
                    tensor.at<float>(0, 0) += weightF * pow(derivativeX.at<float>(p + i - center, q + j - center), 2);
                    tensor.at<float>(0, 1) += weightF * derivativeX.at<float>(p + i - center, q + j - center) * derivativeY.at<float>(p + i - center, q + j - center);
                    tensor.at<float>(1, 0) += weightF * derivativeX.at<float>(p + i - center, q + j - center) * derivativeY.at<float>(p + i - center, q + j - center);
                    tensor.at<float>(1, 1) += weightF * pow(derivativeY.at<float>(p + i - center, q + j - center), 2);
                }

            float R = cv::determinant(tensor) - alpha * pow(cv::trace(tensor)[0], 2);
            resultOper.at<float>(i, j) = R;
        }
    
    cv::Mat sortedMat = resultOper.clone();
    sortedMat = sortedMat.reshape(1,1);
    cv::sort(sortedMat, sortedMat, cv::SORT_ASCENDING + cv::SORT_EVERY_ROW);
    
    float threshold;
    int iter = 0;
    for (int i = 0; i < sortedMat.total(); i++)
        if (sortedMat.at<float>(i) > 0)
            iter++;
    if (iter == 0)
        iter = 1;

    threshold = sortedMat.at<float>(sortedMat.total() - iter + iter * 0.75);
    if(threshold < 254)
        threshold = 254;
    cv::Mat localMaximaMat(resultOper.rows, resultOper.cols, CV_32FC1);
    localMaxima(resultOper, localMaximaMat, squareSize, threshold);
    return localMaximaMat;
}

void feat::Corner::localMaxima(cv::Mat src, cv::Mat& dst, int squareSize, float threshold) { //https://stackoverflow.com/a/13438917/9304781
    if (squareSize == 0) {
        dst = src.clone();
        return;
    }

    cv::Mat m0;
    dst = src.clone();
    cv::Point maxLoc(0, 0);
    
    int sqrCenter = (squareSize - 1) / 2;

    cv::Mat localWindowMask = cv::Mat::zeros(cv::Size(squareSize, squareSize), CV_32F);
    localWindowMask.at<float>(sqrCenter, sqrCenter) = 255;
    
    cv::threshold(dst, m0, threshold, 1, cv::THRESH_BINARY);
    dst = dst.mul(m0);

    for (int i = 0; i < dst.rows; i++)
        for (int j = 0; j < dst.cols; j++) {
            if (dst.at<float>(i, j) == 0)
                continue;

            if (j - sqrCenter < 0 || i - sqrCenter < 0 || i + sqrCenter + 1 >= dst.rows || j + sqrCenter + 1 >= dst.cols) {
                dst.at<float>(i, j) = 0;
                continue;
            }

            m0 = dst.colRange(j - sqrCenter, j + sqrCenter + 1).rowRange(i - sqrCenter, i + sqrCenter + 1);

            minMaxLoc(m0, NULL, NULL, NULL, &maxLoc);
            if ((maxLoc.x == sqrCenter) && (maxLoc.y == sqrCenter)) {
                m0 = m0.mul(localWindowMask);
                j += sqrCenter;
            }

            else
                m0.at<float>(sqrCenter, sqrCenter) = 0;
        }
}

std::bitset<64> feat::Hash::imageHashing_dHash(cv::Mat const imageMat) {
    cv::Mat imgOper = sim::channelCheck(imageMat);

    if (imgOper.type() != CV_8UC4 && imgOper.type() != CV_8UC3 && imgOper.type() != CV_8UC1)
        throw std::exception("Image has to be unmodified by this program.");

    cv::resize(imgOper, imgOper, cv::Size(9, 8), 0, 0, cv::INTER_LINEAR_EXACT);

    std::bitset<64> bits;
    for(int i = 0; i < imgOper.rows; i++)
        for (int j = 0; j < imgOper.cols - 1; j++) {
            bits[i * imgOper.rows + j] = imgOper.at<uchar>(i, j) > imgOper.at<uchar>(i, j + 1) ? 1 : 0;
        }

    return bits;
}

//https://github.com/opencv/opencv_contrib/blob/master/modules/img_hash/src/phash.cpp
std::bitset<64> feat::Hash::imageHashing_pHash(cv::Mat const imageMat) {
    cv::Mat imgOper = sim::channelCheck(imageMat);

    if (imgOper.type() != CV_8UC4 && imgOper.type() != CV_8UC3 && imgOper.type() != CV_8UC1)
        throw std::exception("Image has to be unmodified by this program.");

    cv::resize(imgOper, imgOper, cv::Size(32, 32), 0, 0, cv::INTER_LINEAR_EXACT);
    imgOper.convertTo(imgOper, CV_32F);
    cv::dct(imgOper, imgOper);
    cv::Mat topLeftDCT;
    imgOper(cv::Rect(0, 0, 8, 8)).copyTo(topLeftDCT);
    topLeftDCT.at<float>(0, 0) = 0;
    float const imgMean = static_cast<float>(cv::mean(topLeftDCT)[0]);
    cv::Mat bitsImg;
    cv::compare(topLeftDCT, imgMean, bitsImg, cv::CMP_GT);
    bitsImg /= 255;
    uchar const* bits_ptr = bitsImg.ptr<uchar>(0);
    std::bitset<64> bits;
    for (size_t i = 0; i < bitsImg.total(); i++) {
        bits[i] = bits_ptr[i] != 0;
    }
    return bits;
}

//Uses hamming distance
int feat::Hash::compareImageHash(cv::Mat const lefthand, cv::Mat const righthand) {
    std::bitset<64> lhand_dhash = imageHashing_dHash(lefthand);
    std::bitset<64> rhand_dhash = imageHashing_dHash(righthand);

    int diff = 0;
    for (int i = 0; i < 64; i++) {
        if (lhand_dhash[i] != rhand_dhash[i])
            diff++;
    }

    if (diff < 10) {
        std::bitset<64> lhand_phash = imageHashing_pHash(lefthand);
        std::bitset<64> rhand_phash = imageHashing_pHash(righthand);
        diff = 0;
        for (int i = 0; i < 64; i++) {
            if (lhand_phash[i] != rhand_phash[i])
                diff++;
        }
    }

    return diff;
}

XXH64_hash_t feat::Hash::hash_xxHash(cv::Mat const inputMat) {
    cv::Mat matOper = inputMat.clone();
    XXH64_hash_t hash = XXH64(matOper.ptr<float>(0), matOper.total(), NULL);
    return hash;
}

XXH64_hash_t feat::Hash::setHash(std::vector<cv::Mat>* matVec, std::vector<float>* floatVec) {
    cv::Mat1f hashMat;
    if(matVec != nullptr)
        for (cv::Mat iter : *matVec) {
            iter = iter.reshape(1, iter.total());
            hashMat.push_back(iter);
        }

    if(floatVec != nullptr)
        for (float iter : *floatVec) {
            hashMat.push_back(iter);
        }

    if (!hashMat.isContinuous())
        hashMat = hashMat.clone();

    return feat::Hash::hash_xxHash(hashMat);
}

XXH64_hash_t feat::Hash::setHash(std::vector<std::string> strVec) {
    std::string combined = "";
    for (std::string i : strVec)
        combined.append(i);
    return XXH64(&combined, combined.size(), NULL);
}