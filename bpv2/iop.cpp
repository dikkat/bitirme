#include "iop.h"

int iop::minkorder = 5;

bool iop::checkVectorEmpty(vecf operand) {
	if (operand.empty())
		return true;

	float sum = 0;
	for (auto i : operand)
		sum += i;

	if (gen::cmpf(sum, 0, 0.0005))
		return true;

	return false;
}

std::pair<vecf, bool> iop::Comparison::calculateHistogramSimilarity(feat::Histogram* lh, feat::Histogram* rh, 
	int flagsim, vecf weightVec) { //PARALLELISE
	if ((lh == nullptr || rh == nullptr) && checkVectorEmpty(weightVec))
		return std::make_pair(vecf{}, true);
	if ((lh == nullptr || rh == nullptr) && !checkVectorEmpty(weightVec))
		throw std::exception("If feature is null, it's weight must be null aswell.");

	cv::Mat &lnMat = lh->getNormalizedHistogramMat(); 
	cv::Mat &rnMat = rh->getNormalizedHistogramMat();

	if (lnMat.channels() != rnMat.channels() && lnMat.channels() != weightVec.size())
		throw std::exception("Number of matrix channels must be equal between histogram matrices and to weight vector size.");

	std::vector<cv::Mat> lmatVec;
	cv::split(lnMat, lmatVec);
	std::vector<cv::Mat> rmatVec;
	cv::split(rnMat, rmatVec);

	vecf simVec;
	bool dir;
	for (int i = 0; i < lmatVec.size(); i++) {
		auto similarity = calculateVectorSimilarity(sim::matElementsToVector<float>(lmatVec[i]), 
			sim::matElementsToVector<float>(rmatVec[i]), flagsim);
		simVec.push_back(similarity.first * weightVec[i]);
		dir = similarity.second;
	}

	return std::make_pair(simVec, dir);
}

std::pair<float, bool> iop::calculateVectorSimilarity(vecf const lh, vecf const rh, int flag) {
	float foper;
	bool direction;

	if (lh.size() != rh.size())
		throw std::exception("Vectors to compare must have equal size.");

	switch (flag) {
	case SIM_COSSIM:
		foper = sim::similarityCosine(lh, rh);
		direction = true;
		break;
	case SIM_MANDIST:
		foper = sim::distanceManhattan(lh, rh);
		direction = false;
		break;
	case SIM_EUCDIST:
		foper = sim::distanceEuclidean(lh, rh);
		direction = false;
		break;
	case SIM_MINKDIST: // DONT USE THIS WITHOUT SETTING MINK ORDER FIRST //YEAH LIKE I KEEP TRACK OF THAT
		foper = sim::distanceMinkowski(lh, rh, getMinkowskiOrder());
		direction = false;
		break;
	case SIM_CSQDIST:
		foper = sim::distanceChiSquared(lh, rh);
		direction = false;
		break;
	case SIM_JACSIM:
		foper = sim::similarityJaccard(lh, rh);
		direction = true;
		break;
	case SIM_HISINTR:
		foper = sim::histogramIntersection(lh, rh);
		direction = true;
		break;
	case SIM_CROCOR:
		foper = sim::crossCorrelation(lh, rh);
		direction = true;
		break;
	default:
		throw std::exception("Illegal vector similarity calculation flag.");
	}
	return std::make_pair(foper, direction);
}

std::pair<vecf, bool> iop::Comparison::calculateEdgeSimilarity(feat::Edge* lh, feat::Edge* rh, vecf weightVec, 
	int flagsim, int magbin, int dirbin) {
	if ((lh == nullptr || rh == nullptr) && checkVectorEmpty(weightVec))
		return std::make_pair(vecf{}, true);
	if ((lh == nullptr || rh == nullptr) && !checkVectorEmpty(weightVec))
		throw std::exception("If feature is null, it's weight must be null aswell.");
	if (magbin == 0 || dirbin == 0)
		throw std::exception("Number of bins for gradient features can't be null.");

	auto const &lhGrad = lh->getGradientPtr()->getGradientMats();
	auto const &rhGrad = rh->getGradientPtr()->getGradientMats();

	auto const& lhMagMat = lhGrad.first;
	auto const& rhMagMat = rhGrad.first;

	auto lhMagHist = feat::Histogram(lhMagMat, HIST_DATA, magbin);
	auto rhMagHist = feat::Histogram(rhMagMat, HIST_DATA, magbin);

	auto magSim = calculateHistogramSimilarity(&lhMagHist, &rhMagHist, flagsim, vecf{ 1 });

	auto const& lhDirMat = lhGrad.second;
	auto const& rhDirMat = rhGrad.second;

	auto lhDirHist = feat::Histogram(lhDirMat, HIST_DATA, dirbin);
	auto rhDirHist = feat::Histogram(rhDirMat, HIST_DATA, dirbin);

	auto dirSim = calculateHistogramSimilarity(&lhDirHist, &rhDirHist, flagsim, vecf{ 1 });

	vecf result = { magSim.first[0], dirSim.first[0] };
	return std::make_pair(result, magSim.second);
}

float iop::Comparison::calculateHashSimilarity(feat::Hash* lh, feat::Hash* rh, vecf weightVec) {
	if ((lh == nullptr || rh == nullptr) && weightVec.empty())
		return -1;
	if ((lh == nullptr || rh == nullptr) && !weightVec.empty())
		throw std::exception("If feature is null, it's weight must be null aswell.");

	auto& const lhandHashes = lh->getHashVariables();
	auto& const rhandHashes = rh->getHashVariables();

	auto& const lhand_dhash = lhandHashes.first;
	auto& const rhand_dhash = rhandHashes.first;
	int ddiff = 0;
	if (lhandHashes.first != NULL) {
		for (int i = 0; i < 64; i++) {
			if (lhand_dhash[i] != rhand_dhash[i])
				ddiff++;
		}
	}

	auto& const lhand_phash = lhandHashes.second;
	auto& const rhand_phash = rhandHashes.second;
	int rdiff = 0;
	if (lhandHashes.second != NULL) {
		for (int i = 0; i < 64; i++) {
			if (lhand_phash[i] != rhand_phash[i])
				rdiff++;
		}
	}

	return static_cast<float>(ddiff) * weightVec[0] + static_cast<float>(rdiff) * weightVec[1];
}

int iop::getMinkowskiOrder() {
	return iop::minkorder;
}
void iop::setMinkowskiOrder(int value) {
	iop::minkorder = value;
	return;
}

//EDGE, HISTGRAY, HISTBGR, HISTHSV, DHASH, PHASH
iop::FeatureVector::FeatureVector(img::Image* image, feat::Edge* edge, feat::Histogram* hist_gray,
	feat::Histogram* hist_bgr, feat::Histogram* hist_hsv, feat::Hash* perc_hash) 
	: image(image), edge(edge), hist_gray(hist_gray),
	hist_bgr(hist_bgr), hist_hsv(hist_hsv), perc_hash(perc_hash) {}

//EDGEVEC, HISTGRAYF, HISTBGRVEC, HISTHSVVEC, DHASHF, PHASHF
iop::WeightVector::WeightVector(std::vector<vecf> wvv_total) : wvv_total(wvv_total) {
	auto sizeError = []() {
		throw std::exception("Illegal weight vector element size.");
	};

	float sum = 0;
	for (auto i : wvv_total)
		for (auto j : i)
			sum += j;
	if (!gen::cmpf(sum, 1, 0.05))
		throw std::exception("Weight vector elements' sum total must be 1.");

	if (wvv_total.size() != 5)
		throw std::exception("Weight vector size must be 5.");

	if (wvv_total[0].size() == 2)
		wv_grad = wvv_total[0];
	else
		sizeError();
	if (wvv_total[1].size() == 1)
		w_hgray = wvv_total[1][0];
	else
		sizeError();
	if (wvv_total[2].size() == 3)
		wv_hbgr = wvv_total[2];
	else
		sizeError();
	if (wvv_total[3].size() == 3)
		wv_hhsv = wvv_total[3];
	else
		sizeError();
	if (wvv_total[4].size() == 3)
		wv_hash = wvv_total[4];
	else
		sizeError();
}

iop::WeightVector::WeightVector(vecf* wv_grad, float* w_hgray, vecf* wv_hbgr, vecf* wv_hhsv, vecf* wv_hash) {
	float sum = 0;
	if (wv_grad) {
		this->wv_grad = *wv_grad;
		for (auto i : *wv_grad) sum += i;
	}
	if (w_hgray) {
		this->w_hgray = *w_hgray;
		sum += *w_hgray;
	}
	if (wv_hbgr) {
		this->wv_hbgr = *wv_hbgr;
		for (auto i : *wv_hbgr) sum += i;
	}
	if (wv_hhsv) {
		this->wv_hhsv = *wv_hhsv;
		for (auto i : *wv_hhsv) sum += i;
	}
	if (wv_hash) {
		this->wv_hash = *wv_hash;
		for (auto i : *wv_hash) sum += i;
	}
	if (!gen::cmpf(sum, 1, 0.05))
		throw std::exception("Weight vector elements' sum total must be 1.");
}

iop::WeightVector::WeightVector(float* w_grad, float* w_hgray, float* w_hbgr, float* w_hhsv, 
	float* w_hash) {
	float sum = 0;
	if (w_grad) {
		float per = *w_grad / 2;
		this->wv_grad = { per, per };
		sum += *w_grad;
	}
	if (w_hgray) {
		this->w_hgray = *w_hgray;
		sum += *w_hgray;
	}
	if (w_hbgr) {
		float per = *w_hbgr / 3;
		this->wv_hbgr= { per, per, per };
		sum += *w_hbgr;
	}
	if (w_hhsv) {
		float per = *w_hhsv / 3;
		this->wv_hhsv = { per, per, per };
		sum += *w_hhsv;
	}
	if (w_hash) {
		float per = *w_hash / 2;
		this->wv_hash = { per, per };
		sum += *w_hash;
	}
	if (!gen::cmpf(sum, 1, 0.05))
		throw std::exception("Weight vector elements' sum total must be 1.");
}

std::vector<iop::Comparison> iop::Comparator::beginMultiCompare(FeatureVector* source, std::vector<std::vector<string>> destVec, WeightVector* wvec, int cmpNum) {
	this->source = source;
	auto thrNum = std::thread::hardware_concurrency();

	std::vector<std::thread> threadVec;
	for (int i = 0; i < thrNum; i++) threadVec.push_back(std::thread());

	std::vector<Comparison> cmpVec;

	auto wnullError = [](bool check) {
		check ? throw std::exception("If feature is not null, it's weight must not be null.")
			: throw std::exception("If weight is not null, source image's corresponding feature must not be null.");
	};

	std::vector<bool> thrCond;
	for (int i = 0; i < thrNum; i++) thrCond.push_back(true);

	int num = -1;
	auto threadCounter = [&num, &thrNum]() {
		if (num == thrNum - 1)
			num = 0;
		else
			num++;
		return num;
	};

	std::function<void(int, string, FeatureVector*, WeightVector*)> task;

	std::mutex m;

	std::vector<std::vector<vecf>> timeVec;
	for (int i = 0; i < thrNum; i++) timeVec.push_back(std::vector<vecf>());
	for (auto& i : timeVec) {
		for (int j = 0; j < 8; j++) i.push_back(vecf());
	}

	task = [&wnullError, &m, &cmpVec, &thrCond, &cmpNum, &timeVec](int i, string str, FeatureVector* source,
		WeightVector* wvec) {
			if (cmpVec.size() > cmpNum) {
				thrCond[i] = true;
				return;
			}

			img::Image img(str, cv::IMREAD_COLOR);

			feat::Edge* edge = nullptr;
			if (source->edge != nullptr) {
				if (wvec->wv_grad.empty())
					wnullError(true);
				edge = new feat::Edge(img.getImageMat(), source->edge->getEdgeFlag(), source->edge->getCannyPtr(),
					source->edge->getComparisonValues()[0], source->edge->getComparisonValues()[1],
					source->edge->getComparisonValues()[2]);
			}
			else if (!wvec->wv_grad.empty())
				wnullError(false);

			feat::Histogram* hist_gray = nullptr;
			if (source->hist_gray != nullptr) {
				if (gen::cmpf(wvec->w_hgray, 0))
					wnullError(true);
				hist_gray = new feat::Histogram(img.getImageMat(), HIST_GRAY, source->hist_gray->getBin()[0]);
			}
			else if (!gen::cmpf(wvec->w_hgray, 0))
				wnullError(false);

			feat::Histogram* hist_bgr = nullptr;
			if (source->hist_bgr != nullptr) {
				if (wvec->wv_hbgr.empty())
					wnullError(true);
				hist_bgr = new feat::Histogram(img.getImageMat(), HIST_BGR, source->hist_bgr->getBin()[0],
					source->hist_bgr->getBin()[1], source->hist_bgr->getBin()[2]);
			}
			else if (!wvec->wv_hbgr.empty())
				wnullError(false);
			
			feat::Histogram* hist_hsv = nullptr;
			if (source->hist_hsv != nullptr) {
				if (wvec->wv_hhsv.empty())
					wnullError(true);
				hist_hsv = new feat::Histogram(img.getImageMat(), HIST_HSV, source->hist_hsv->getBin()[0],
					source->hist_hsv->getBin()[1], source->hist_hsv->getBin()[2]);
			}
			else if (!wvec->wv_hhsv.empty())
				wnullError(false);
			
			feat::Hash* perc_hash = nullptr;
			if (source->perc_hash != nullptr) {
				if (wvec->wv_hash.empty())
					wnullError(true);
				perc_hash = new feat::Hash(img.getImageMat(), std::make_pair(
					source->perc_hash->getHashVariables().first != NULL ? true : false,
					source->perc_hash->getHashVariables().second != NULL ? true : false));
			}
			else if (!wvec->wv_hash.empty())
				wnullError(false);

			FeatureVector rhand(&img, edge, hist_gray, hist_bgr, hist_hsv, perc_hash);

			m.lock();

			if (cmpVec.size() >= cmpNum) {
				m.unlock();
				delete(edge);
				delete(hist_gray);
				delete(hist_bgr);
				delete(hist_hsv);
				delete(perc_hash);
				thrCond[i] = true;
				return;
			}

			cmpVec.push_back(Comparison(source, &rhand, wvec));

			m.unlock();
			delete(edge);
			delete(hist_gray);
			delete(hist_bgr);
			delete(hist_hsv);
			delete(perc_hash);

			thrCond[i] = true;
			return;
	};

	for (int j = 0; j < destVec[0].size(); j++) {
		auto str = destVec[0][j];
		while (true) {
			if (cmpVec.size() >= cmpNum)
				break;
			int i = threadCounter();
			if (thrCond[i] == true) {
				thrCond[i] = false;
				if (threadVec[i].joinable())
					threadVec[i].join();
				threadVec[i] = std::thread{ task, i, str, source, wvec };
				break;
			}
			
		}
	}

	for (int i = 0; i < thrCond.size(); i++) {
		if (thrCond[i] != true)
			i = 0;
	}

	for (int i = 0; i < thrNum; i++) {
		if (threadVec[i].joinable()) {
			threadVec[i].join();
		}
	}

	this->dest = cmpVec;
	return cmpVec;
}

iop::Comparison::Comparison(FeatureVector* source, FeatureVector* rhand, WeightVector* wvec) {
	source = new FeatureVector(*source);
	rhand = new FeatureVector(*rhand);
	wvec = new WeightVector(*wvec);

	source_dir = source->image->getVariablesString()[1];
	rhand_dir = rhand->image->getVariablesString()[1];

	float noOfFeats = 0;

	auto tempg = calculateEdgeSimilarity(source->edge, rhand->edge, wvec->wv_grad, SIM_EUCDIST, 255, 16).first;
	if (!tempg.empty()) {
		diff_gradm = tempg[0];
		euc_dist += pow(diff_gradm, 2);
		diff_gradd = tempg[1];
		euc_dist += pow(diff_gradd, 2);
		noOfFeats += 2;
	}

	auto temphg = calculateHistogramSimilarity(source->hist_gray, rhand->hist_gray, SIM_EUCDIST, vecf{ wvec->w_hgray }).first;
	if (!temphg.empty()) {
		diff_hgray = temphg[0];
		euc_dist += diff_hgray;
		noOfFeats++;
	}

	auto temphbgr = calculateHistogramSimilarity(source->hist_bgr, rhand->hist_bgr, SIM_EUCDIST, wvec->wv_hbgr).first;
	if (!temphbgr.empty()) {
		diff_hbgrb = temphbgr[0];
		euc_dist += pow(diff_hbgrb, 2);
		diff_hbgrg = temphbgr[1];
		euc_dist += pow(diff_hbgrg, 2);
		diff_hbgrr = temphbgr[2];
		euc_dist += pow(diff_hbgrr, 2);
		noOfFeats += 3;
	}

	auto temphhsv = calculateHistogramSimilarity(source->hist_hsv, rhand->hist_hsv, SIM_EUCDIST, wvec->wv_hhsv).first;
	if (!temphhsv.empty()) {
		diff_hhsvh = temphhsv[0];
		euc_dist += pow(diff_hhsvh, 2);
		diff_hhsvs = temphhsv[1];
		euc_dist += pow(diff_hhsvs, 2);
		diff_hhsvv = temphhsv[2];
		euc_dist += pow(diff_hhsvv, 2);
		noOfFeats += 3;
	}

	diff_hash = calculateHashSimilarity(source->perc_hash, rhand->perc_hash, wvec->wv_hash);
	if (diff_hash != -1) {
		euc_dist += pow(diff_hash, 2);
		euc_dist = sqrt(euc_dist);
		noOfFeats += 2;
	}
	
	euc_dist = pow(euc_dist, 1 / noOfFeats);

	delete(source);
	delete(rhand);
	delete(wvec);
}

std::pair<string, string> iop::Comparison::getDirValues() {
	return std::make_pair(this->source_dir, this->rhand_dir);
}

float iop::Comparison::getEuclideanDistance() {
	return this->euc_dist;
}

std::vector<iop::Comparison> iop::Comparator::getComparisonVector(bool sorted) {
	auto temp = this->dest;
	if(sorted)
		std::sort(temp.begin(), temp.end(), [](Comparison lh, Comparison rh) {
			return lh.euc_dist < rh.euc_dist;
		});
	return temp;
}