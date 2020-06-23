#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <filesystem>
#include "GeneralOperations.h"
#include "SimilarityMeasures.h"
#include "ImageOperations.h"

#define CALC_BGRHIST 1
#define CALC_HSVHIST 2
#define CALC_GRAYHIST 3

namespace img {
	class Histogram;
	class Edge;

	class Image {
	public:
		Image(std::string imgdir, int flag);
		Image(cv::Mat sourcemat);
		Image(cv::Mat sourcemat, std::string imgdir);
		Image() {}
		const std::string getImageName();
		cv::Mat getImageMat();
		void destroyImageHist();
		void setImageHist(int fb, int sb, int tb, int flag);
		void setImageDirectory(std::string imgdir);
		Histogram* getImageHist();
	private:
		cv::Mat imagemat;
		std::vector<Edge*> imageEdgeList;
		Histogram* imagehist = NULL;
		std::string name, dir;
		std::string buildImageName(std::string imgdir);
		cv::Mat readImageFile(std::string imgdir, int flag);
	};	

	class Histogram {
	public:
		Histogram(Image* srimg, int fb, int sb, int tb, int flag);
		Histogram() {}
		cv::Mat getHistogramMat();
		cv::Mat getNormalizedHistogramMat();
		cv::Mat getSourceMat();
		int* getBin();
		void setHistogramDisplayImage(int w, int h);
	private:
		cv::Mat histmat, nhistmat;
		int fbin, sbin, tbin;
		Image* histdspimg;
		Image* srcimg;
		std::vector<cv::Mat> histBGR; //[0] BLUE, [1] GREEN, [2] RED
		std::vector<cv::Mat> nhistBGR;
		cv::Mat histogramHSVCalculation(cv::Mat sourcemat);
		cv::Mat histogramBGRCalculation(cv::Mat sourcemat);
		cv::Mat histogramGRAYCalculation(cv::Mat sourcemat);
		cv::Mat normalizeMat(cv::Mat sourcemat, float alpha, float beta);
		std::vector<cv::Mat> histogramBGRSeparateCalculation(cv::Mat sourcemat);
		Image createHistogramDisplayImage(std::vector<cv::Mat> bgrhist, int hist_w, int hist_h);
	};

	class Edge {
	public:
		Edge(Image* srimg, int flag);
		Image* getSourceImg();
	private:
		Image* srcimg;
	};

	std::vector<Image> readImageFolder(std::string imagefoldername, int flag, bool all, int number);
	std::string typeToString(int type);
	void displayImage(cv::Mat imgmat);
}