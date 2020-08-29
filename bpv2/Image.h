#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <filesystem>
#include <string>
#include <ctime>
#include "gen.h"
#include "sim.h"
#include "iop.h"

namespace img {
	class Image {
	public:
		Image(std::string imgdir, int flag);
		Image(cv::Mat sourcemat);
		Image(cv::Mat sourcemat, std::string imgdir);
		Image() {}
		const std::string getImageName();
		cv::Mat getImageMat();
	private:
		cv::Mat imageMat;
		std::string name, dir;
		std::string buildImageName(std::string imgdir);
		void setImageDirectory(std::string imgdir);
		cv::Mat readImageFile(std::string imgdir, int flag);
	};

	std::vector<Image> readImageFolder(std::string imagefoldername, int flag, bool all, int number);
	std::string typeToString(int type);
	void displayImage(cv::Mat imgmat);
}