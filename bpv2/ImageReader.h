#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <filesystem>
#include "GeneralOperations.h"


namespace ir {
	extern int unconfiletotal;
	cv::Mat readImageFile(std::string imagedirectory, int flag); 
	std::vector<cv::Mat> readImageFolder(std::string imagefoldername, int flag);
}