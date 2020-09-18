#include "iop.h"

int iop::minkorder;

void iop::setDatabaseClass(dbop::Database dbObj) {
	iop_dbPtr = &dbObj;
}

float iop::calculateHistogramSimilarity(img::Image& lefthand, img::Image& righthand, int fb, int sb, int tb, int flaghist, int flagsim) { //PARALLELISE
	feat::Histogram *lhist, *rhist;
	while (true) {
		try {
			lhist = new feat::Histogram(lefthand.getImageMat(), flaghist, fb, sb, tb);
			rhist = new feat::Histogram(righthand.getImageMat(), flaghist, fb, sb, tb);
			iop_dbPtr->insert_ImageHistogram(lefthand.getHash(), lhist->getHash());
			iop_dbPtr->insert_ImageHistogram(righthand.getHash(), rhist->getHash());
			iop_dbPtr->insert_Histogram(*lhist);
			break;
		}
		catch (std::exception e) {
			std::string err_str = e.what();
			if (err_str.find("UNIQUE") != std::string::npos);
			else
				throw std::exception(e);
			continue;
		}
	}

	std::vector<float> lvecoperator = sim::matElementsToVector<float>(lhist->getNormalizedHistogramMat());
	std::vector<float> rvecoperator = sim::matElementsToVector<float>(rhist->getNormalizedHistogramMat());
	
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

