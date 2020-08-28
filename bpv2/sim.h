#pragma once
#define _USE_MATH_DEFINES

#include <cmath>
#include <chrono>
#include <iostream>
#include <vector>
#include <math.h>
#include <valarray>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/gapi.hpp>
#include "gen.h"

using cf = std::complex<float>;

namespace sim {
	
	class Convolution {
	public:
		void sim::Convolution::fftRosetta(std::valarray<cf>& x);
		void sim::Convolution::ifftRosetta(std::valarray<cf>& x);
		static cv::Mat convolution2D(cv::Mat image, cv::Mat kernel);
		static cv::Mat Convolution::convolution2DopenCV(cv::Mat image, cv::Mat kernel);
		cv::Mat convertToComplexMat(cv::Mat imageMat);	
		//constexpr cf operator()(const cf& Left, const cf& Right) const { //COPIED OPERATOR FROM STD::MULTIPLIES CLASS
		//	return fcm(Left, Right);
		//}	
		void zeroPadding(std::vector<cf>& a, int power = -1);
		static std::vector<cf> signalMultiplication(std::vector<cf> const lh, std::vector<cf> const rh);
		void fastFourierTransform2D(std::vector<cf>& a, bool invert);
	private:
		//cv::Mat fastFourierTransform_2D(cv::Mat const image);
		//void fft(std::vector<cf>& a, bool invert);
		//constexpr cf fcm(const cf& lh, const cf& rh) const;
	};

	template<typename T>
	bool vectorSizeEqualityCheck(std::vector<T> lefthand, std::vector<T> righthand);

	float similarityCosine(std::vector<float> lefthand, std::vector<float> righthand);
	float similarityJaccard(std::vector<float> lefthand, std::vector<float> righthand);
	float distanceManhattan(std::vector<float> lefthand, std::vector<float> righthand);
	float distanceEuclidean(std::vector<float> lefthand, std::vector<float> righthand);
	float distanceMinkowski(std::vector<float> lefthand, std::vector<float> righthand, int order);
	float distanceChiSquared(std::vector<float> lefthand, std::vector<float> righthand);
	float histogramIntersection(std::vector<float> lefthand, std::vector<float> righthand);
	float crossCorrelation(std::vector<float> lefthand, std::vector<float> righthand);

	template <typename T>
	std::vector<T> matToVector(cv::Mat operand);
	template <typename T>
	cv::Mat vectorToMat(std::vector<T> operand);
	template <typename T>
	std::vector<T> matElementsToVector(cv::Mat operand);
	template <typename T>
	cv::Mat vectorToMatElementsRowMajor(std::vector<T> operand, int mrows, int mcols, int mtype);

	cv::Mat convolution2D(cv::Mat const image, cv::Mat kernel);
	cv::Mat convolution2DSeparable(cv::Mat const image, cv::Mat kernel);
	cv::Mat convolution2DNormal(cv::Mat const image, cv::Mat kernel);
	cv::Mat convolution2DHelix(cv::Mat const image, cv::Mat kernel);

	cv::Mat rotateMatrix180(cv::Mat srcmat);
	cv::Mat filterGauss(cv::Mat const operand, int k = 5, float sigma = 1.4, float mu = 1, bool openCV = true);
	cv::Mat gaussKernel(float kernel_size, float sigma, float mu);

	float sumOfVectorMembers(std::vector<float> operand, int offset);
	int rankOfMatrix(cv::Mat mat);
	template <typename T>
	int getI(std::vector<T> operand);
	cv::Mat channelCheck(cv::Mat const image);
}

template <typename T>
std::vector<T> operator+(const std::vector<T>& a, const std::vector<T>& b);