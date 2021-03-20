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

enum image_flag{IMG_EMPTY};

namespace feat {
	class Hash;
}

class TableModel;

namespace img {
	class Image {
	public:
		Image(string imgdir, int flag);
		Image();
		Image(const Image& other);
		Image& operator=(const Image& other) {
			this->hash = other.hash;
			this->imageMat = other.imageMat.clone();
			this->name = other.name;
			this->dir = other.dir;
			return *this;
		}
		const string getImageName();
		cv::Mat getImageMat();
		std::vector<string> getVariablesString() const;
		XXH64_hash_t getHash() const;
		void correctDir();
	private:
		friend class TableModel;
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