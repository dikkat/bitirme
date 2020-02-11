#pragma once
#include "Image.h"
#include "SimilarityMeasures.h"

namespace iop {
	float calculateCosineSimilarity(img::Image lefthand, img::Image righthand, int hb, int sb);
}