#pragma once
#include "feat.h"
#include <algorithm>

namespace lnkr {
	void destroyImageHist();
	static void setImageHist(img::Image* image, int fb, int sb, int tb, int flag);
	void setImageDirectory(std::string imgdir);
	feat::Histogram* getImageHist(img::Image* image);
	void setImageHist(int fb, int sb, int tb, int flag);
}