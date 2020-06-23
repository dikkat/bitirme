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

float iop::calculateHistogramSimilarity(img::Image& lefthand, img::Image& righthand, int fb, int sb, int tb, int flaghist, int flagsim) { //PARALLELISE
	if (!histogramEqualityCheck(lefthand, fb, sb, tb))
		lefthand.setImageHist(fb, sb, tb, flaghist);

	if (!histogramEqualityCheck(righthand, fb, sb, tb))
		righthand.setImageHist(fb, sb, tb, flaghist);

	std::vector<float> lvecoperator = sim::matToVector<float>(lefthand.getImageHist()->getNormalizedHistogramMat());
	std::vector<float> rvecoperator = sim::matToVector<float>(righthand.getImageHist()->getNormalizedHistogramMat());
	
	cv::Mat xde = sim::vectorToMat(lvecoperator);

	/*for (int i = 0; i < 4; i++) {
		float xd = (float)lefthand.getImageHist()->getNormalizedHistogramMat().data[i];
		float xda = (float)xde.data[i];
		gen::tout << xda << "\t"
			<< lvecoperator[i] << "\t"
			<< lefthand.getImageHist()->getNormalizedHistogramMat().at<float>(0, 0, i) << "\t"
			<< xd << "\t"
			<< xde.at<float>(i, 0)
			<< std::endl;
	}*/

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
	case SIM_CSQDIST:
		iopoperator = sim::distanceChiSquared(lvecoperator, rvecoperator);
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
	return iopoperator;
}

cv::Mat iop::calculateEdge(img::Image& lefthand, int flag) {
	return lefthand.getImageMat();
}

int iop::getMinkowskiOrder() {
	return iop::minkorder;
}
void iop::setMinkowskiOrder(int value) {
	iop::minkorder = value;
	return;
}

