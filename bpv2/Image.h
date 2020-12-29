#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <filesystem>
#include <string>
#include <ctime>
#include "xxhash.h"
#include "feat.h"

namespace feat {
	class Hash;
}

namespace img {
	class Image {
	public:
		Image(string imgdir, int flag);
		Image() {}
		const string getImageName();
		cv::Mat getImageMat();
		std::vector<string> getVariablesString();
		XXH64_hash_t getHash();		
	private:
		XXH64_hash_t hash;
		cv::Mat imageMat;
		string name, dir;
		void setImageDirectory(string imgdir);
		cv::Mat readImageFile(string imgdir, int flag);
	};

	class Icon {
	public:
		Icon(cv::Mat iconMat_src);
		cv::Mat getIconMat();
		XXH64_hash_t getHash();
	private:
		XXH64_hash_t hash;
		cv::Mat iconMat;
	};

	string buildImageName(string imgdir);
	std::vector<Image> readImageFolder(string imagefoldername, int flag, bool all, int number);
	string typeToString(int type);
	void displayImage(cv::Mat imgmat);

	void imageTesting(cv::Mat imageMat, string filename);
}