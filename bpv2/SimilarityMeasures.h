#pragma once
#include <iostream>
#include <vector>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "GeneralOperations.h"

namespace sim {
	bool vectorEqualityCheck(std::vector<float> lefthand, std::vector<float> righthand);
	float similarityCosine(std::vector<float> lefthand, std::vector<float> righthand);
	float similarityJaccard(std::vector<float> lefthand, std::vector<float> righthand);
	float distanceManhattan(std::vector<float> lefthand, std::vector<float> righthand);
	float distanceEuclidean(std::vector<float> lefthand, std::vector<float> righthand);
	float distanceMinkowski(std::vector<float> lefthand, std::vector<float> righthand, int order);
	float histogramIntersection(std::vector<float> lefthand, std::vector<float> righthand);
	float crossCorrelation(std::vector<float> lefthand, std::vector<float> righthand);
	float sumOfVectorMembers(std::vector<float> operand);
	std::vector<float> matToVector(cv::Mat operand);
}