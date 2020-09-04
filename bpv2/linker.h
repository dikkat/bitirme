#pragma once
#include "feat.h"
#include "image.h"
#include "dbop.h"
#include <algorithm>

namespace lnkr {
	void destroyImageHist();
	static void setImageHist(img::Image* imageMat, int fb, int sb, int tb, int flag);
	void setImageDirectory(std::string imgdir);
	feat::Histogram* getImageHist(img::Image* imageMat);
	void setImageHist(int fb, int sb, int tb, int flag);
}