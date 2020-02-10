#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <filesystem>
#include "GeneralOperations.h"

namespace img {
	class Histogram;

	class Image {
	public:
		Image(std::string imgdir, int flag);
		Image(cv::Mat sourcemat);
		Image() {}
		std::string getImageName();
		cv::Mat getImageMat();
		void setImageHist(int hb, int sb);
		Histogram* getImageHist();
	private:
		cv::Mat imagemat;
		Histogram* imagehist = NULL;
		std::string name, dir;
		std::string buildImageName(std::string imgdir);
		cv::Mat readImageFile(std::string imgdir, int flag);
	};	

	class Histogram {
	public:
		Histogram(Image* srimg, int hb, int sb);
		Histogram() {}
		cv::Mat getHistogramMat();
		cv::Mat getNormalizedHistogramMat();
		cv::Mat getSourceMat();
		int* getBin();
		void setHistogramDisplayImage(int w, int h);
	private:
		cv::Mat histmat, nhistmat;
		int hbin, sbin;
		Image* histdspimg;
		Image* srcimg;
		std::vector<cv::Mat> histBGR; //[0] BLUE, [1] GREEN, [2] RED
		std::vector<cv::Mat> nhistBGR;
		cv::Mat histogramCalculation(cv::Mat sourcemat);
		cv::Mat normalizeMat(cv::Mat sourcemat, float alpha, float beta);
		std::vector<cv::Mat> histogramBGRCalculation(cv::Mat sourcemat);
		img::Image createHistogramDisplayImage(std::vector<cv::Mat> bgrhist, int hist_w, int hist_h);
	};

	std::vector<Image> readImageFolder(std::string imagefoldername, int flag, bool all, int number);
}