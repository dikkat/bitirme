#pragma once
#include "image.h"
#include "sim.h"
#include "feat.h"
#include "dbop.h"
#include <QList>

enum hist_cmpr_flag { SIM_COSSIM, SIM_MANDIST, SIM_EUCDIST, SIM_MINKDIST, SIM_JACSIM, SIM_HISINTR, SIM_CROCOR, SIM_CSQDIST };


namespace img {
	class Image;
}

static QList<img::Image> imageList;

static dbop::Database* iop_dbPtr;

namespace iop {
	void setDatabaseClass(dbop::Database dbObj);
	bool histogramEqualityCheck(feat::Histogram operand, int fb, int sb, int tb);
	float calculateHistogramSimilarity(img::Image& lefthand, img::Image& righthand, int fb, int sb, int tb, int flaghist, int flagsim);
	cv::Mat calculateEdge(img::Image& lefthand, int flag);
	int getMinkowskiOrder();
	void setMinkowskiOrder(int value);
	extern int minkorder;
}