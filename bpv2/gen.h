#pragma once

#include <fstream>
#include <sstream>
#include <algorithm>
#include <functional>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/gapi.hpp>
#include <boost/filesystem/operations.hpp>


namespace gen {
	extern std::ostringstream tout;
	void imageTesting(cv::Mat imageMat, std::string filename);
	void printTesting(std::ostringstream &osoperator);
	bool cmpf(float A, float B, float epsilon = 0.005f);
}

