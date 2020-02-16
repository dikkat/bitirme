#pragma once
#include "Image.h"
#include "SimilarityMeasures.h"

#define SIM_COSSIM 1
#define SIM_MANDIST 2
#define SIM_EUCDIST 3
#define SIM_MINKDIST 4
#define SIM_JACSIM 5
#define SIM_HISINTR 6
#define SIM_CROCOR 7

namespace iop {
	bool histogramEqualityCheck(img::Image operand, int fb, int sb, int tb);
	float calculateSimilarity(img::Image lefthand, img::Image righthand, int fb, int sb, int tb, int flaghist, int flagsim);
	int getMinkowskiOrder();
	void setMinkowskiOrder(int value);
	extern int minkorder;
}