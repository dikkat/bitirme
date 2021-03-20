#pragma once
#define _USE_MATH_DEFINES

#include "gen.h"

using cf = std::complex<float>;

namespace sim {
	template<typename T>
	bool vectorSizeEqualityCheck(std::vector<T> lefthand, std::vector<T> righthand);

	float similarityCosine(vecf lefthand, vecf righthand);
	float similarityJaccard(vecf lefthand, vecf righthand);
	float distanceManhattan(vecf lefthand, vecf righthand);
	float distanceEuclidean(vecf lefthand, vecf righthand);
	float distanceMinkowski(vecf lefthand, vecf righthand, int order);
	float distanceChiSquared(vecf lefthand, vecf righthand);
	float histogramIntersection(vecf lefthand, vecf righthand);
	float crossCorrelation(vecf lefthand, vecf righthand);

	template <typename T>
	std::vector<T> matToVector(const cv::Mat& operand);
	template <typename T>
	cv::Mat vectorToMat(std::vector<T> operand);
	template <typename T>
	std::vector<T> matElementsToVector(const cv::Mat& operand);
	template <typename T>
	cv::Mat vectorToMatElementsRowMajor(std::vector<T> operand, int mrows, int mcols, int mtype);

	cv::Mat convolution2D(const cv::Mat& imageMat, const cv::Mat& kernel);
	cv::Mat convolution2DSeparable(const cv::Mat& imageMat, const cv::Mat& kernel);
	cv::Mat convolution2DNormal(const cv::Mat& imageMat, const cv::Mat& kernel);
	cv::Mat convolution2DHelix(const cv::Mat& imageMat, const cv::Mat& kernel);
	cv::Mat convolution2DOpenCV(const cv::Mat& imageMat, const cv::Mat& kernel);

	cv::Mat rotateMatrix180(const cv::Mat& srcmat);
	cv::Mat filterGauss(const cv::Mat& operand, int k = 5, float sigma = 1.4, float mu = 0, bool openCV = true);
	cv::Mat gaussKernel(float kernel_size, float sigma, float mu);

	float sumOfVectorMembers(vecf operand, int offset);
	template <typename T>
	int getI(std::vector<T> operand);
	cv::Mat channelCheck(const cv::Mat& imageMat);
}