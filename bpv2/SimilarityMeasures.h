#pragma once
#include <iostream>
#include <vector>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "GeneralOperations.h"

namespace sim {
	float cosineSimilarity(std::vector<float> lefthand, std::vector<float> righthand);
	float sumOfVectorMembers(std::vector<float> operand);
	std::vector<float> matToVector(cv::Mat operand);
}