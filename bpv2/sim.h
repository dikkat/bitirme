#pragma once
#define _USE_MATH_DEFINES

#include <cmath>
#include <chrono>
#include <iostream>
#include <vector>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/gapi.hpp>
#include "gen.h"

using cf = std::complex<float>;

namespace sim {
	class EdgeDetectorCanny {
	public:
		EdgeDetectorCanny() : kernelsize(3), sigma(10), mu(0), thigh(0.13), tlow(0.075), weakratio(0.09),
			kernelx((cv::Mat_<float>(3, 3) << 1, 0, -1, 1, 0, -1, 1, 0, -1)), 
			kernely((cv::Mat_<float>(3, 3) << 1, 0, 1, 0, 0, 0, -1, 0, -1)) {}

		EdgeDetectorCanny(float ksize, float s, float m, float th, float tl, float wr, cv::Mat kx, cv::Mat ky) : 
			kernelsize(ksize), sigma(s), mu(m), thigh(th), tlow(tl), weakratio(wr), kernelx(kx), kernely(ky) {}

		static cv::Mat edgeDetectionCanny(cv::Mat const imageMat, sim::EdgeDetectorCanny edcOperator = sim::EdgeDetectorCanny());

	private:
		float kernelsize;
		float thigh;
		float tlow;
		float sigma;
		float mu;
		float weakratio;
		cv::Mat kernelx;
		cv::Mat kernely;
		cv::Mat nonMaximumSuppression(cv::Mat& dirMat, cv::Mat& magMat);
		void performHysteresis(cv::Mat& resultMat, float weak, float strong);
		void doubleThreshold(cv::Mat& resultMat, cv::Mat const magMat, float max, float tlow, float thigh, float weakratio);
	}; //HAUSDORFF DISTANCE FOR EDGE COMPARISON ALSO MAYBE SIFT ALGORITHM

	/*
	class Convolution {
	public:
		static cv::Mat convolution2D(cv::Mat image, cv::Mat kernel);
		static cv::Mat convertToComplexMat(cv::Mat imageMat);
		constexpr cf operator()(const cf& Left, const cf& Right) const { //COPIED OPERATOR FROM STD::MULTIPLIES CLASS
			return fcm(Left, Right);
		}
	private:
		//cv::Mat fastFourierTransform_2D(cv::Mat const image);
		//void fft(std::vector<cf>& a, bool invert);
		void zeroPadding(std::vector<cf>& a, int power = -1);
		constexpr cf fcm(const cf& lh, const cf& rh) const;
		std::vector<cf> signalMultiplication(std::vector<cf> const lh, std::vector<cf> const rh);
		void fastFourierTransform2D(std::vector<cf>& a, bool invert);
	};
	*/

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

	cv::Mat edgeDetectionSobel(cv::Mat const imageMat);
	cv::Mat edgeDetectionPrewitt(cv::Mat const imageMat);
	cv::Mat edgeDetectionRobertsCross(cv::Mat const imageMat);
	cv::Mat commonOperationsSPR(cv::Mat const kernelx, cv::Mat const kernely, cv::Mat const imat);
	std::vector<cv::Mat> calculateEdgeGradientMagnitudeDirection(cv::Mat const kx, cv::Mat const ky, cv::Mat const imat);

	template <typename T>
	std::vector<T> matToVector(cv::Mat operand);
	template <typename T>
	cv::Mat vectorToMat(std::vector<T> operand);
	template <typename T>
	std::vector<T> matElementsToVector(cv::Mat operand);
	template <typename T>
	cv::Mat vectorToMatElementsRowMajor(std::vector<T> operand, int mrows, int mcols, int mtype);

	cv::Mat convolution2D(cv::Mat const image, cv::Mat kernel);
	//cv::Mat convolution2DSeparable(cv::Mat const image, cv::Mat kernel);
	//cv::Mat convolution2DNormal(cv::Mat const image, cv::Mat kernel);
	cv::Mat convolution2DHelix(cv::Mat const image, cv::Mat kernel);

	cv::Mat rotateMatrix180(cv::Mat srcmat);
	cv::Mat filterGauss(cv::Mat const operand, int k, float sigma, float mu);
	std::vector<float> gaussKernel(float kernel_size, float sigma, float mu);

	float sumOfVectorMembers(std::vector<float> operand, int offset);
	int rankOfMatrix(cv::Mat mat);
	template <typename T>
	int getI(std::vector<T> operand);
}

template <typename T>
std::vector<T> operator+(const std::vector<T>& a, const std::vector<T>& b);