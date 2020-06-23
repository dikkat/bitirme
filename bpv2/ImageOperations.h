#pragma once
#include "Image.h"
#include "SimilarityMeasures.h"
#include <QList>

#define SIM_COSSIM 1
#define SIM_MANDIST 2
#define SIM_EUCDIST 3
#define SIM_MINKDIST 4
#define SIM_JACSIM 5
#define SIM_HISINTR 6
#define SIM_CROCOR 7
#define SIM_CSQDIST 8


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