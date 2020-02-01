#include "imagereader.h"

int ir::unconfiletotal;

cv::Mat ir::readImageFile(std::string imagedirectory, int flag) {
	if (flag != cv::IMREAD_UNCHANGED && flag != cv::IMREAD_COLOR && flag != cv::IMREAD_GRAYSCALE)
		throw std::exception("ILLEGAL IMAGE READ FLAG");
	cv::Mat matoperator;
	matoperator = cv::imread(imagedirectory, flag);
	if (!matoperator.data)
		throw std::exception("CANT READ THE FILE");
	return matoperator;
}

std::vector<cv::Mat> ir::readImageFolder(std::string imagefolderdirectory, int flag) {
	if (imagefolderdirectory.find(".") != std::string::npos)
		throw std::exception("ILLEGAL FOLDER PATH");
	else if (flag != cv::IMREAD_UNCHANGED && flag != cv::IMREAD_COLOR && flag != cv::IMREAD_GRAYSCALE)
		throw std::exception("ILLEGAL IMAGE READ FLAG");
	cv::Mat matoperator;
	std::string pathstring;
	std::vector<cv::Mat> vecoperator;
	ir::unconfiletotal = 0;
	int i = 0;
	for (const auto& entry : std::filesystem::directory_iterator(imagefolderdirectory)) {
		if (i == 10)
			break;
		i++;
		pathstring = entry.path().u8string();
		matoperator = cv::imread(pathstring, flag);
		if (!matoperator.data) {
			ir::unconfiletotal++;
			continue;
		}
		vecoperator.push_back(matoperator);
	}
	return vecoperator;
}