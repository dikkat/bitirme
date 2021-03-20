#pragma once
#include "image.h"
#include "sim.h"
#include "feat.h"
#include <mutex>

enum cmpr_flag { SIM_COSSIM, SIM_MANDIST, SIM_EUCDIST, SIM_MINKDIST, SIM_JACSIM, SIM_HISINTR, SIM_CROCOR, SIM_CSQDIST };

namespace img {
	class Image;
}

namespace iop {
	bool checkVectorEmpty(vecf operand);
	std::pair<float, bool> calculateVectorSimilarity(const vecf& lh, const vecf& rh, int flag);
	int getMinkowskiOrder();
	void setMinkowskiOrder(int value);
	extern int minkorder;

	class FeatureVector {
	public:
		FeatureVector() {}
		FeatureVector(bool def_fv);
		//EDGE, HISTGRAY, HISTBGR, HISTHSV, DHASH, PHASH
		FeatureVector(img::Image* image, feat::Edge* edge, feat::Histogram* hist_gray,
			feat::Histogram* hist_bgr, feat::Histogram* hist_hsv, feat::Hash* perc_hash)
			: image(image ? new img::Image(*image) : nullptr), edge(edge ? new feat::Edge(*edge) : nullptr), 
			hist_gray(hist_gray ? new feat::Histogram(*hist_gray) : nullptr), 
			hist_bgr(hist_bgr ? new feat::Histogram(*hist_bgr) : nullptr), 
			hist_hsv(hist_hsv ? new feat::Histogram(*hist_hsv) : nullptr),
			perc_hash(perc_hash ? new feat::Hash(*perc_hash) : nullptr) {}
		~FeatureVector();
		FeatureVector(const FeatureVector& other) : FeatureVector(other.image, other.edge,
			other.hist_gray, other.hist_bgr, other.hist_hsv, other.perc_hash) {}
		FeatureVector& operator=(const FeatureVector& other) {
			if (this == &other)
				return *this;
			this->image = new img::Image(*other.image);
			this->edge = other.edge ? new feat::Edge(*other.edge) : nullptr;
			this->hist_gray = other.hist_gray ? new feat::Histogram(*other.hist_gray) : nullptr;
			this->hist_bgr = other.hist_bgr ? new feat::Histogram(*other.hist_bgr) : nullptr;
			this->hist_hsv = other.hist_hsv ? new feat::Histogram(*other.hist_hsv) : nullptr;
			this->perc_hash = other.perc_hash ? new feat::Hash(*other.perc_hash) : nullptr;
			return *this;
		}
		img::Image* image = nullptr;
		feat::Edge* edge = nullptr;
		feat::Histogram *hist_gray = nullptr, *hist_bgr = nullptr, *hist_hsv = nullptr;
		feat::Hash* perc_hash = nullptr;
	};
	
	class WeightVector {
	public:
		WeightVector(bool equal) : WeightVector(&equalf, &equalf, &equalf, &equalf, &equalf) {}
		WeightVector(vecf* wv_grad = nullptr, float* w_hgray = nullptr, vecf* wv_hbgr = nullptr, 
			vecf* wv_hhsv = nullptr, vecf* wv_hash = nullptr);
		WeightVector(float* w_grad = nullptr, float* w_hgray = nullptr, float* w_hbgr = nullptr,
			float* w_hhsv = nullptr, float* w_hash = nullptr);
		WeightVector(WeightVector& other) : WeightVector(&other.wv_grad, &other.w_hgray, &other.wv_hbgr,
			&other.wv_hhsv, &other.wv_hash) {}
		WeightVector& operator=(const WeightVector& other) {
			this->w_hgray = other.w_hgray;
			this->wv_grad = other.wv_grad;
			this->wv_hbgr = other.wv_hbgr;
			this->wv_hhsv = other.wv_hhsv;
			this->wv_hash = other.wv_hash;
			this->wvv_total = { &this->wv_grad, new vecf{this->w_hgray}, &this->wv_hbgr, &this->wv_hhsv, &this->wv_hash };
			return *this;
		}
		float equalf = 0.2;
		float w_hgray = 0;
		vecf wv_grad, wv_hbgr, wv_hhsv, wv_hash;
		std::vector<vecf*> wvv_total = { &wv_grad, new vecf{w_hgray}, &wv_hbgr, &wv_hhsv, &wv_hash };
		bool vectorAtIndexIsZero(int index);
	};

	class Comparison {
	public:
		Comparison() {}
		Comparison(FeatureVector* source, FeatureVector* rhand, WeightVector* wvec);
		Comparison(const Comparison& other);
		friend class Comparator;
		FeatureVector* source = nullptr;
		FeatureVector* rhand = nullptr;
		WeightVector* wvec = nullptr;
		string source_dir;
		string rhand_dir;
		float euc_dist = 0;
		float diff_gradm = -1, diff_gradd = -1, diff_hgray = -1, diff_hbgrb = -1, diff_hbgrg = -1, 
			diff_hbgrr = -1, diff_hhsvh = -1, diff_hhsvs = -1, diff_hhsvv = -1, diff_hashd = -1,
			diff_hashp = -1;
		std::vector<float*> diff_total = { &diff_gradm, &diff_gradd, &diff_hgray, &diff_hbgrb, &diff_hbgrg, &diff_hbgrr, 
			&diff_hhsvh, &diff_hhsvs, &diff_hhsvv, &diff_hashd, &diff_hashp, &euc_dist };
		Comparison& operator=(const Comparison& other) {
			this->source_dir = other.source_dir;
			this->rhand_dir = other.rhand_dir;
			this->euc_dist = other.euc_dist;
			this->diff_gradm = other.diff_gradm;
			this->diff_gradd = other.diff_gradd;
			this->diff_hgray = other.diff_hgray;
			this->diff_hbgrb = other.diff_hbgrb;
			this->diff_hbgrg = other.diff_hbgrg;
			this->diff_hbgrr = other.diff_hbgrr;
			this->diff_hhsvh = other.diff_hhsvh;
			this->diff_hhsvs = other.diff_hhsvs;
			this->diff_hhsvv = other.diff_hhsvv;
			this->diff_hashd = other.diff_hashd;
			this->diff_hashp = other.diff_hashp;
			this->diff_total.clear();
			this->diff_total = { &this->diff_gradm, &this->diff_gradd, &this->diff_hgray, &this->diff_hbgrb,
				&this->diff_hbgrg, &this->diff_hbgrr, &this->diff_hhsvh, &this->diff_hhsvs,
				&this->diff_hhsvv, &this->diff_hashd, &this->diff_hashp, &this->euc_dist };
			return *this;
		}
		std::pair<string, string> getDirValues();
		float getEuclideanDistance();
		void calculateEuclideanDistance();
		std::pair<vecf, bool> calculateHistogramSimilarity(feat::Histogram* lh, feat::Histogram* rh,
			int flagsim);
		std::pair<vecf, bool> calculateEdgeSimilarity(feat::Edge* lh, feat::Edge* rh,
			int flagsim, int magbin, int dirbin);
		vecf calculateHashSimilarity(feat::Hash* lhand, feat::Hash* rhand);
	};

	class Comparator {
	public:
		Comparator() {}
		std::vector<Comparison> beginMultiCompare(FeatureVector* source, std::vector<string> destVec,
			WeightVector* wvec, int cmpNum);
		std::vector<Comparison> getComparisonVector(bool sorted);
		std::vector<Comparison>* getComparisonVector_ptr();
		WeightVector* getWeightVector();
	private:
		FeatureVector* source = nullptr;
		std::vector<Comparison> dest;
		WeightVector* wvec = nullptr;
	};
	
}