#include "ImageOperations.h"

int iop::minkorder;

bool iop::histogramEqualityCheck(img::Image operand, int fb, int sb, int tb) {
	if (operand.getImageHist() == NULL ||
		operand.getImageHist()->getBin()[0] != fb ||
		operand.getImageHist()->getBin()[1] != sb ||
		operand.getImageHist()->getBin()[2] != tb)
		return false;
	else
		return true;
}

float iop::calculateSimilarity(img::Image lefthand, img::Image righthand, int fb, int sb, int tb, int flaghist, int flagsim) {
	if (!histogramEqualityCheck(lefthand, fb, sb, tb))
		lefthand.setImageHist(fb, sb, tb, flaghist);

	if (!histogramEqualityCheck(righthand, fb, sb, tb))
		righthand.setImageHist(fb, sb, tb, flaghist);

	std::vector<float> lvecoperator = sim::matToVector(lefthand.getImageHist()->getNormalizedHistogramMat());
	std::vector<float> rvecoperator = sim::matToVector(righthand.getImageHist()->getNormalizedHistogramMat());

	float iopoperator;

	switch (flagsim) {
	case SIM_COSSIM:
		iopoperator = sim::similarityCosine(lvecoperator, rvecoperator);
		break;
	case SIM_MANDIST:
		iopoperator = sim::distanceManhattan(lvecoperator, rvecoperator);
		break;
	case SIM_EUCDIST:
		iopoperator = sim::distanceEuclidean(lvecoperator, rvecoperator);
		break;
	case SIM_MINKDIST: // DONT USE THIS WITHOUT SETTING MINK ORDER FIRST
		iopoperator = sim::distanceMinkowski(lvecoperator, rvecoperator, getMinkowskiOrder());
		break;
	case SIM_JACSIM:
		iopoperator = sim::similarityJaccard(lvecoperator, rvecoperator);
		break;
	case SIM_HISINTR:
		iopoperator = sim::histogramIntersection(lvecoperator, rvecoperator);
		break;
	case SIM_CROCOR:
		iopoperator = sim::crossCorrelation(lvecoperator, rvecoperator);
		break;
	default:
		throw std::exception("Illegal similarity calculation flag.");
	}
	delete(lefthand.getImageHist());
	delete(righthand.getImageHist());
	return iopoperator;
}

int iop::getMinkowskiOrder() {
	return iop::minkorder;
}
void iop::setMinkowskiOrder(int value) {
	iop::minkorder = value;
	return;
}