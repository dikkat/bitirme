#pragma once
#include "feat.h"
#include <algorithm>

namespace lnkr {
	void destroyImageHist();
	static void setImageHist(img::Image* image, int fb, int sb, int tb, int flag);
	void setImageDirectory(std::string imgdir);
	feat::Histogram* getImageHist(img::Image* image);

	class ImageFeatures {
	public:
		img::Image* getSrcImg();
		ImageFeatures(img::Image* srimg);
		void setImageHist(int fb, int sb, int tb, int flag);
		feat::Histogram* getImageHist();
	private:
		feat::Histogram* imagehist = NULL;
		img::Image* srcimg;
		std::vector<feat::Edge*> imageEdgeList;
	};

	extern std::vector<ImageFeatures> ImgFeatVector;

	int queryImgFeatVector(img::Image* image);
}