#pragma once
#include "image.h"
#include "sim.h"
#include <QList>

enum hist_cmpr_flag { SIM_COSSIM, SIM_MANDIST, SIM_EUCDIST, SIM_MINKDIST, SIM_JACSIM, SIM_HISINTR, SIM_CROCOR, SIM_CSQDIST };


namespace img {
	class Image;
}

static QList<img::Image> imageList;

namespace iop {
	bool histogramEqualityCheck(img::Image operand, int fb, int sb, int tb);
	float calculateHistogramSimilarity(img::Image& lefthand, img::Image& righthand, int fb, int sb, int tb, int flaghist, int flagsim);
	cv::Mat calculateEdge(img::Image& lefthand, int flag);
	int getMinkowskiOrder();
	void setMinkowskiOrder(int value);
	extern int minkorder;
}