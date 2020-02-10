#include "ImageOperations.h"

float iop::calculateCosineSimilarity(img::Image lefthand, img::Image righthand, int hb, int sb) {
	if (lefthand.getImageHist() == NULL || 
		lefthand.getImageHist()->getBin()[0] != hb ||
		lefthand.getImageHist()->getBin()[1] != sb)
		lefthand.setImageHist(hb, sb);

	if (righthand.getImageHist() == NULL ||
		righthand.getImageHist()->getBin()[0] != hb ||
		righthand.getImageHist()->getBin()[1] != sb)
		righthand.setImageHist(hb, sb);

	std::vector<float> lvecoperator = sim::matToVector(lefthand.getImageHist()->getNormalizedHistogramMat());
	std::vector<float> rvecoperator = sim::matToVector(righthand.getImageHist()->getNormalizedHistogramMat());

	float iopoperator = sim::cosineSimilarity(lvecoperator, rvecoperator);
	return iopoperator;
}