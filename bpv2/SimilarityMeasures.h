#pragma once
#define _USE_MATH_DEFINES

#include <cmath>
#include "GeneralOperations.h"
#include <iostream>
#include <vector>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/gapi.hpp>

namespace sim {
	struct edgeDetectorCanny {
	public:

		edgeDetectorCanny() : kernelsize(5), sigma(1), mu(0), thigh(220), tlow(thigh/3), kernelx((cv::Mat_<float>(3, 3) << 1, 0, -1, 2, 0, -2, 1, 0, -1)), kernely((cv::Mat_<float>(3, 3) << 1, 2, 1, 0, 0, 0, -1, -2, -1)) {}
		edgeDetectorCanny(float ksize, float s, float m, float th, float tl, cv::Mat kx, cv::Mat ky) : kernelsize(ksize), sigma(s), mu(m), thigh(th), tlow(tl), kernelx(kx), kernely(ky) {}
		float kernelsize;
		float thigh;
		float tlow;
		float sigma;
		float mu;
		cv::Mat kernelx;
		cv::Mat kernely;
	};

	bool vectorSizeEqualityCheck(std::vector<float> lefthand, std::vector<float> righthand);

	float similarityCosine(std::vector<float> lefthand, std::vector<float> righthand);
	float similarityJaccard(std::vector<float> lefthand, std::vector<float> righthand);
	float distanceManhattan(std::vector<float> lefthand, std::vector<float> righthand);
	float distanceEuclidean(std::vector<float> lefthand, std::vector<float> righthand);
	float distanceMinkowski(std::vector<float> lefthand, std::vector<float> righthand, int order);
	float distanceChiSquared(std::vector<float> lefthand, std::vector<float> righthand);
	float histogramIntersection(std::vector<float> lefthand, std::vector<float> righthand);
	float crossCorrelation(std::vector<float> lefthand, std::vector<float> righthand);

	cv::Mat edgeDetectionCanny(cv::Mat const imageMat, edgeDetectorCanny edcOperator = edgeDetectorCanny());
	cv::Mat edgeDetectionSobel(cv::Mat const imageMat);
	cv::Mat edgeDetectionPrewitt(cv::Mat const imageMat);
	cv::Mat edgeDetectionRobertsCross(cv::Mat const imageMat);
	cv::Mat commonOperationsSPR(cv::Mat const kernelx, cv::Mat const kernely, cv::Mat const imat);
	std::vector<cv::Mat> calculateEdgeGradientMagnitudeDirection(cv::Mat const kx, cv::Mat const ky, cv::Mat const imat);

	template <typename T>
	std::vector<T> matToVector(cv::Mat operand);
	template <typename T>
	cv::Mat vectorToMat(std::vector<T> operand);

	cv::Mat convolution2D(cv::Mat image, cv::Mat kernel);
	cv::Mat rotateMatrix180(cv::Mat srcmat);
	cv::Mat filterGauss(cv::Mat const operand, int k, float sigma, float mu);
	std::vector<float> gaussKernel(float kernel_size, float sigma, float mu);

	float sumOfVectorMembers(std::vector<float> operand);
}

template <typename T>
std::vector<T> operator+(const std::vector<T>& a, const std::vector<T>& b);