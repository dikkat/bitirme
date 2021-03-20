#include "iop.h"

int iop::minkorder = 5;

bool iop::checkVectorEmpty(vecf operand) {
	if (operand.empty())
		return true;

	float sum = 0;
	std::accumulate(operand.begin(), operand.end(), sum);

	if (gen::cmpf(sum, 0, 0.0005))
		return true;

	return false;
}

std::pair<vecf, bool> iop::Comparison::calculateHistogramSimilarity(feat::Histogram* lh, 
	feat::Histogram* rh, int flagsim) {
	if (lh == nullptr || rh == nullptr)
		return std::make_pair(vecf{}, true);

	cv::Mat &lnMat = lh->getNormalizedHistogramMat(); 
	cv::Mat &rnMat = rh->getNormalizedHistogramMat();

	if (lnMat.channels() != rnMat.channels())
		throw std::exception("Number of matrix channels must be equal between histogram matrices.");

	std::vector<cv::Mat> lmatVec;
	cv::split(lnMat, lmatVec);
	std::vector<cv::Mat> rmatVec;
	cv::split(rnMat, rmatVec);

	vecf simVec;
	bool dir;
	for (int i = 0; i < lmatVec.size(); i++) {
		auto similarity = calculateVectorSimilarity(sim::matElementsToVector<float>(lmatVec[i]), 
			sim::matElementsToVector<float>(rmatVec[i]), flagsim);
		simVec.push_back(similarity.first);
		dir = similarity.second;
	}

	return std::make_pair(simVec, dir);
}

std::pair<float, bool> iop::calculateVectorSimilarity(const vecf& lh, const vecf& rh, int flag) {
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

std::pair<vecf, bool> iop::Comparison::calculateEdgeSimilarity(feat::Edge* lh, feat::Edge* rh,
	int flagsim, int magbin, int dirbin) {
	if (lh == nullptr || rh == nullptr)
		return std::make_pair(vecf{}, true);
	if (magbin == 0 && dirbin == 0)
		throw std::exception("Number of bins for both gradient features can't be null.");

	auto const &lhGrad = lh->getGradientPtr()->getGradientMats();
	auto const &rhGrad = rh->getGradientPtr()->getGradientMats();

	auto const& lhMagMat = lhGrad.first;
	auto const& rhMagMat = rhGrad.first;

	auto lhMagHist = feat::Histogram(lhMagMat, HIST_DATA, magbin);
	auto rhMagHist = feat::Histogram(rhMagMat, HIST_DATA, magbin);

	auto magSim = calculateHistogramSimilarity(&lhMagHist, &rhMagHist, flagsim);

	auto const& lhDirMat = lhGrad.second;
	auto const& rhDirMat = rhGrad.second;

	auto lhDirHist = feat::Histogram(lhDirMat, HIST_DATA, dirbin);
	auto rhDirHist = feat::Histogram(rhDirMat, HIST_DATA, dirbin);

	auto dirSim = calculateHistogramSimilarity(&lhDirHist, &rhDirHist, flagsim);

	vecf result = { magSim.first[0], dirSim.first[0] };
	return std::make_pair(result, magSim.second);
}

vecf iop::Comparison::calculateHashSimilarity(feat::Hash* lh, feat::Hash* rh) {
	if (lh == nullptr || rh == nullptr)
		return vecf{};

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

	return vecf{ static_cast<float>(ddiff), static_cast<float>(rdiff) };
}

int iop::getMinkowskiOrder() {
	return iop::minkorder;
}
void iop::setMinkowskiOrder(int value) {
	iop::minkorder = value;
	return;
}

iop::FeatureVector::~FeatureVector() {
	delete(this->image);
	delete(this->edge);
	delete(this->hist_gray);
	delete(this->hist_bgr);
	delete(this->hist_hsv);
	delete(this->perc_hash);
}

iop::WeightVector::WeightVector(vecf* wv_grad, float* w_hgray, vecf* wv_hbgr, vecf* wv_hhsv, vecf* wv_hash) {
	float sum = 0;
	if (wv_grad) {
		this->wv_grad = *wv_grad;
		std::accumulate(wv_grad->begin(), wv_grad->end(), sum);
	}
	else {
		this->wv_grad = vecf(2, 0);
	}

	if (w_hgray) {
		this->w_hgray = *w_hgray;
		sum += *w_hgray;
		delete(this->wvv_total[1]);
		this->wvv_total[1] = new vecf{ this->w_hgray };
	}

	if (wv_hbgr) {
		this->wv_hbgr = *wv_hbgr;
		std::accumulate(wv_hbgr->begin(), wv_hbgr->end(), sum);
	}
	else {
		this->wv_hbgr = vecf(3, 0);
	}

	if (wv_hhsv) {
		this->wv_hhsv = *wv_hhsv;
		std::accumulate(wv_hhsv->begin(), wv_hhsv->end(), sum);
	}
	else {
		this->wv_hhsv = vecf(3, 0);
	}

	if (wv_hash) {
		this->wv_hash = *wv_hash;
		std::accumulate(wv_hash->begin(), wv_hash->end(), sum);
	}
	else {
		this->wv_hash = vecf(2, 0);
	}

	if (!gen::cmpf(sum, 0) && !gen::cmpf(sum, 1, 0.05))
		throw std::exception("Weight vector elements' sum total must be 1.");
}

iop::WeightVector::WeightVector(float* w_grad, float* w_hgray, float* w_hbgr, float* w_hhsv, 
	float* w_hash) : wvv_total(std::vector<vecf*>(5, new vecf{})) {
	float sum = 0;
	if (w_grad) {
		float per = *w_grad / 2;
		wv_grad = { per, per };
		sum += *w_grad;
	}
	else {
		wv_grad = vecf(2, 0);
	}

	if (w_hgray) {
		this->w_hgray = *w_hgray;
		sum += *w_hgray;
		delete(wvv_total[1]);
		wvv_total[1] = new vecf{ this->w_hgray };
	}

	if (w_hbgr) {
		float per = *w_hbgr / 3;
		wv_hbgr= { per, per, per };
		sum += *w_hbgr;
	}
	else {
		wv_hbgr = vecf(3, 0);
	}

	if (w_hhsv) {
		float per = *w_hhsv / 3;
		wv_hhsv = { per, per, per };
		sum += *w_hhsv;
	}
	else {
		wv_hhsv = vecf(3, 0);
	}

	if (w_hash) {
		float per = *w_hash / 2;
		wv_hash = { per, per };
		sum += *w_hash;
	}
	else {
		wv_hash = vecf(2, 0);
	}

	if (!gen::cmpf(sum, 0) && !gen::cmpf(sum, 1, 0.05))
		throw std::exception("Weight vector elements' sum total must be 1.");
}

std::vector<iop::Comparison> iop::Comparator::beginMultiCompare(FeatureVector* source, 
	std::vector<string> destVec, WeightVector* wvec, int cmpNum) {
	this->wvec = new WeightVector(*wvec);
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

	std::function<void(int, const string&, FeatureVector*, WeightVector*)> task;

	std::mutex m;

	std::vector<std::vector<vecf>> timeVec;
	for (int i = 0; i < thrNum; i++) timeVec.push_back(std::vector<vecf>());
	for (auto& i : timeVec) {
		for (int j = 0; j < 8; j++) i.push_back(vecf());
	}

	task = [&](int i, const string& str, FeatureVector* source, WeightVector* wvec) {
		if (cmpVec.size() > cmpNum) {
			thrCond[i] = true;
			return;
		}

		img::Image img(str, cv::IMREAD_COLOR);

		feat::Edge* edge = nullptr;
		if (source->edge && !source->edge->empty()) {
			if (wvec->vectorAtIndexIsZero(0))
				wnullError(true);
			edge = new feat::Edge(img.getImageMat(), source->edge->getEdgeFlag(), source->edge->getCannyPtr(),
				source->edge->getComparisonValues()[0], source->edge->getComparisonValues()[1],
				source->edge->getComparisonValues()[2]);
		}
		else if (!wvec->vectorAtIndexIsZero(0))
			wnullError(false);

		feat::Histogram* hist_gray = nullptr;
		if (source->hist_gray && !source->hist_gray->empty()) {
			if (wvec->vectorAtIndexIsZero(1))
				wnullError(true);
			hist_gray = new feat::Histogram(img.getImageMat(), HIST_GRAY, source->hist_gray->getBin()[0]);
		}
		else if (!wvec->vectorAtIndexIsZero(1))
			wnullError(false);

		feat::Histogram* hist_bgr = nullptr;
		if (source->hist_bgr && !source->hist_bgr->empty()) {
			if (wvec->vectorAtIndexIsZero(2))
				wnullError(true);
			hist_bgr = new feat::Histogram(img.getImageMat(), HIST_BGR, source->hist_bgr->getBin()[0],
				source->hist_bgr->getBin()[1], source->hist_bgr->getBin()[2]);
		}
		else if (!wvec->vectorAtIndexIsZero(2))
			wnullError(false);
			
		feat::Histogram* hist_hsv = nullptr;
		if (source->hist_hsv && !source->hist_hsv->empty()) {
			if (wvec->vectorAtIndexIsZero(3))
				wnullError(true);
			hist_hsv = new feat::Histogram(img.getImageMat(), HIST_HSV, source->hist_hsv->getBin()[0],
				source->hist_hsv->getBin()[1], source->hist_hsv->getBin()[2]);
		}
		else if (!wvec->vectorAtIndexIsZero(3))
			wnullError(false);
			
		feat::Hash* perc_hash = nullptr;
		if (source->perc_hash && !source->perc_hash->empty()) {
			if (wvec->vectorAtIndexIsZero(4))
				wnullError(true);
			perc_hash = new feat::Hash(img.getImageMat(), std::make_pair(
				source->perc_hash->getHashVariables().first != NULL ? true : false,
				source->perc_hash->getHashVariables().second != NULL ? true : false));
		}
		else if (!wvec->vectorAtIndexIsZero(4))
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

	for (int j = 0; j < destVec.size(); j++) {
		auto str = destVec[j];
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
	this->source = new FeatureVector(*source);
	rhand = new FeatureVector(*rhand);
	this->wvec = new WeightVector(*wvec);

	source_dir = source->image->getVariablesString()[1];
	rhand_dir = rhand->image->getVariablesString()[1];

	if (source->edge) {
		auto tempg = calculateEdgeSimilarity(source->edge, rhand->edge, SIM_EUCDIST,
			source->edge->getComparisonValues()[1], source->edge->getComparisonValues()[2]).first;
		if (!tempg.empty()) {
			diff_gradm = tempg[0];
			diff_gradd = tempg[1];
		}
	}

	if (source->hist_gray) {
		auto temphg = calculateHistogramSimilarity(source->hist_gray, rhand->hist_gray, SIM_EUCDIST).first;
		if (!temphg.empty()) {
			diff_hgray = temphg[0];
		}
	}

	if (source->hist_bgr) {
		auto temphbgr = calculateHistogramSimilarity(source->hist_bgr, rhand->hist_bgr, SIM_EUCDIST).first;
		if (!temphbgr.empty()) {
			diff_hbgrb = temphbgr[0];
			diff_hbgrg = temphbgr[1];
			diff_hbgrr = temphbgr[2];
		}
	}

	if (source->hist_hsv) {
		auto temphhsv = calculateHistogramSimilarity(source->hist_hsv, rhand->hist_hsv, SIM_EUCDIST).first;
		if (!temphhsv.empty()) {
			diff_hhsvh = temphhsv[0];
			diff_hhsvs = temphhsv[1];
			diff_hhsvv = temphhsv[2];
		}
	}

	if (source->perc_hash) {
		auto temphash = calculateHashSimilarity(source->perc_hash, rhand->perc_hash);
		if (!temphash.empty()) {
			diff_hashd = temphash[0];
			diff_hashp = temphash[1];
		}
	}

	this->diff_total.clear();
	this->diff_total = { &this->diff_gradm, &this->diff_gradd, &this->diff_hgray, &this->diff_hbgrb,
		&this->diff_hbgrg, &this->diff_hbgrr, &this->diff_hhsvh, &this->diff_hhsvs,
		&this->diff_hhsvv, &this->diff_hashd, &this->diff_hashp, &this->euc_dist };

	calculateEuclideanDistance();
	
	delete(this->source);
	delete(rhand);
	delete(this->wvec);
}

void iop::Comparison::calculateEuclideanDistance() {
	auto wvec = this->wvec->wvv_total;
	int k = 0;
	int nooffeat = 0;
	for(int i = 0; i < wvec.size(); i++)
		for (int j = 0; j < wvec[i]->size(); j++) {
			auto curr = *diff_total[k];
			if (!gen::cmpf(curr, -1) && !gen::cmpf(wvec.at(i)->at(j), 0, 0.0005)) {
				euc_dist += pow(curr, 2) * wvec.at(i)->at(j);
				k++;
				nooffeat++;
			}
			else 
				k++;			
		}
	euc_dist = pow(euc_dist, 0.5);
}

iop::Comparison::Comparison(const iop::Comparison& other) {
	*this = other;
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

std::vector<iop::Comparison>* iop::Comparator::getComparisonVector_ptr() {
	return &dest;
}

iop::WeightVector* iop::Comparator::getWeightVector() {
	return this->wvec;
}

bool iop::WeightVector::vectorAtIndexIsZero(int index) {
	try { 
		float sum = 0;
		for (int i = 0; i < wvv_total.at(index)->size(); i++)
			sum += wvv_total.at(index)->at(i);
		return sum == 0;
	}
	catch (const std::out_of_range& e) {
		throw;
	}
}