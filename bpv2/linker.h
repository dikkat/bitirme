#pragma once
#include "feat.h"
#include "image.h"
#include "dbop.h"
#include <algorithm>

static dbop::Database* lnkr_dbPtr;

namespace lnkr {
	void setDatabaseClass(dbop::Database dbObj);
	img::Image createImage(std::string dir, int flag);
	static void setHistogram(img::Image* image_ptr, int flag, int fb, int sb, int tb);
	void setIcon(img::Image* image_ptr);
	feat::Histogram* getImageHist(img::Image* image_ptr);
}