#pragma once
#include "image.h"

enum hist_flag { CALC_BGRHIST, CALC_HSVHIST, CALC_GRAYHIST };

namespace feat {
	class Feature {
	public:
		img::Image* getSourceImg();
	protected:
		img::Image* srcimg;
	};

	class Histogram {
	public:
		Histogram(img::Image* srimg, int fb, int sb, int tb, int flag = 0);
		Histogram(cv::Mat mat, int fb, int sb, int tb, int flag = 0);
		Histogram() {}
		cv::Mat getHistogramMat();
		cv::Mat getNormalizedHistogramMat();
		cv::Mat getSourceMat();
		int* getBin();
		void setHistogramDisplayImage(int w, int h);
	private:
		cv::Mat histmat, nhistmat, srcmat;
		int fbin, sbin, tbin;
		img::Image* histdspimg;
		img::Image* srcimg;
		std::vector<cv::Mat> histBGR; //[0] BLUE, [1] GREEN, [2] RED
		std::vector<cv::Mat> nhistBGR;
		cv::Mat histogramHSVCalculation(cv::Mat sourcemat);
		cv::Mat histogramBGRCalculation(cv::Mat sourcemat);
		cv::Mat histogramGRAYCalculation(cv::Mat sourcemat);
		cv::Mat normalizeMat(cv::Mat sourcemat, float alpha, float beta);
		std::vector<cv::Mat> histogramBGRSeparateCalculation(cv::Mat sourcemat);
		img::Image createHistogramDisplayImage(std::vector<cv::Mat> bgrhist, int hist_w, int hist_h);
	};

	class Edge {
	public:
		Edge(img::Image* srimg, int flag);
		img::Image* getSourceImg();
	private:
		img::Image* srcimg;
	};
}