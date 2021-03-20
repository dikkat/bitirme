#include "gen.h"

std::ostringstream gen::tout;

void gen::printTesting(std::ostringstream &osoperator) {
	std::fstream testfile;
	testfile.open("testbridge.txt", std::ios::out);
	testfile << tout.str();
	testfile.close();
}

bool gen::cmpf(float A, float B, float epsilon) {
	return (fabs(A - B) < epsilon);
}

void gen::imageTesting(const cv::Mat& imageMat, const string& filename) {
	time_t now = time(0);
	tm* ltm = localtime(&now);

	boost::filesystem::create_directory("Resources/test/" + std::to_string(1900 + ltm->tm_year) + "-"
		+ std::to_string(1 + ltm->tm_mon) + "-" + std::to_string(ltm->tm_mday) + "/");
	string stroper = "Resources/test/" + std::to_string(1900 + ltm->tm_year) + "-" 
		+ std::to_string(1 + ltm->tm_mon) + "-" + std::to_string(ltm->tm_mday) + "/"  + std::to_string(ltm->tm_hour) + "-"
		+ std::to_string(ltm->tm_min) + "-" + std::to_string(1 + ltm->tm_sec) + "--" 
		+ filename + ".jpg";	
	cv::imwrite(stroper, imageMat);
}

cv::Mat gen::realNormalize(const cv::Mat& operand, int bins) {
	cv::Mat newMat = cv::Mat::zeros(operand.rows, operand.cols, CV_32FC1);
	double max;
	cv::minMaxLoc(operand, nullptr, &max);
	for (int j = 0; j < operand.total(); j++) {
		double binSize = max / static_cast<double>(bins);
		for (int k = 0; k < bins; k++) {
			if (operand.at<float>(j) >= binSize * k && operand.at<float>(j) <= binSize * (k + 1)) {
				newMat.at<float>(j) = k + 1;
				break;
			}
		}
	}
	return newMat;
}

string gen::format(float f) {
	float error = 0.002;
	if (abs(f - roundf(f)) < error) {
		int i = roundf(f);
		return std::to_string(i);
	}
	else {
		std::stringstream stream;
		stream << std::fixed << std::setprecision(3) << f;
		return stream.str();
	}
}

bool gen::cmpMat(const cv::Mat& lh, const cv::Mat& rh) {
	if (lh.rows == rh.rows && lh.cols == rh.cols)
		return cv::countNonZero(lh != rh) == 0;
	else
		return false;
}

std::string gen::dirnameOf(const std::string& fname) {
	size_t pos = fname.find_last_of("\\/");
	return (std::string::npos == pos)
		? ""
		: fname.substr(0, pos);
}