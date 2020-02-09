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
		cv::Mat getImageMat();
		void setImageHist(int hb, int sb);
		Histogram* getImageHist();
	private:
		cv::Mat imagemat;
		Histogram* imagehist;
		cv::Mat readImageFile(std::string imagedirectory, int flag);
		std::vector<cv::Mat> readImageFolder(std::string imagefoldername, int flag);
	};	

	class Histogram {
	public:
		Histogram(Image* srimg, int hb, int sb);
		Histogram() {}
		cv::Mat getHistogramMat();
		cv::Mat getSourceMat();
		void setHistogramDisplayImage();		
	private:
		cv::Mat histmat, nhistmat;
		int hbin, sbin;
		Image histdspimg, srcimg;
		std::vector<cv::Mat> histBGR; //[0] BLUE, [1] GREEN, [2] RED
		std::vector<cv::Mat> nhistBGR;
		cv::Mat histogramCalculation(cv::Mat sourcemat);
		cv::Mat normalizeMat(cv::Mat sourcemat);
		std::vector<cv::Mat> histogramBGRCalculation(cv::Mat sourcemat);
		img::Image createHistogramDisplayImage(std::vector<cv::Mat> bgrhist);
	};
}