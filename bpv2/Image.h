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
		Image() {}
	private:
		cv::Mat imagemat;
		img::Histogram* imagehist;
		cv::Mat readImageFile(std::string imagedirectory, int flag);
		std::vector<cv::Mat> readImageFolder(std::string imagefoldername, int flag);
	};	

	class Histogram {
	public:
		Histogram(cv::Mat sourcemat);
		Histogram() {}
	private:
		cv::Mat histmat, nhistmat;
		img::Image histim;
		std::vector<cv::Mat> histBGR; //[0] BLUE, [1] GREEN, [2] RED
		cv::Mat histogramCalculation(cv::Mat sourcemat);
		cv::Mat histogramNormalizedCalculation(cv::Mat sourcemat);
		std::vector<cv::Mat> histogramBGRCalculation(cv::Mat sourcemat);
	};
}