#pragma once
#include "image.h"
#include "sim.h"
#include "feat.h"
#include "dbop.h"
#include "linker.h"
#include <QList>

enum cmpr_flag { SIM_COSSIM, SIM_MANDIST, SIM_EUCDIST, SIM_MINKDIST, SIM_JACSIM, SIM_HISINTR, SIM_CROCOR, SIM_CSQDIST };

namespace img {
	class Image;
}

static QList<img::Image> imageList;

static dbop::Database* iop_dbPtr;

namespace iop {
	void setDatabaseClass(dbop::Database dbObj);
	bool checkVectorEmpty(vecf operand);
	std::pair<float, bool> calculateVectorSimilarity(vecf const lh, vecf const rh, int flag);
	int getMinkowskiOrder();
	void setMinkowskiOrder(int value);
	extern int minkorder;

	class FeatureVector {
	public:
		FeatureVector() {}
		FeatureVector(img::Image* image, std::vector<bool>* enabler = nullptr, feat::Edge* edge = nullptr, feat::Histogram* hist_gray = nullptr,
			feat::Histogram* hist_bgr = nullptr, feat::Histogram* hist_hsv = nullptr, feat::Hash* perc_hash = nullptr);
	private:
		friend class Comparator;
		friend class Comparison;
		img::Image* image;
		std::vector<bool>* enabler;
		feat::Edge* edge;
		feat::Histogram *hist_gray, *hist_bgr, *hist_hsv;
		feat::Hash* perc_hash;
	};
	
	class WeightVector {
	public:
		WeightVector() {}
		WeightVector(std::vector<vecf> wvv_total);
		WeightVector(vecf* wv_grad = nullptr, float* w_hgray = nullptr, vecf* wv_hbgr = nullptr, 
			vecf* wv_hhsv = nullptr, vecf* wv_hash = nullptr);
	private:
		friend class Comparator;
		friend class Comparison;
		float w_hgray = 0;
		vecf wv_grad, wv_hbgr, wv_hhsv, wv_hash;
		std::vector<vecf> wvv_total;
	};

	class Comparison {
	public:
		Comparison(FeatureVector* source, FeatureVector* rhand, WeightVector* wvec);
	private:
		friend class Comparator;
		FeatureVector* source;
		FeatureVector* rhand;
		WeightVector* wvec;
		string source_dir;
		string rhand_dir;
		float euc_dist = 0;
		float diff_gradm, diff_gradd, diff_hgray, diff_hbgrb, diff_hbgrg, diff_hbgrr;
		float diff_hhsvh, diff_hhsvs, diff_hhsvv, diff_hash;
		std::pair<vecf, bool> calculateHistogramSimilarity(feat::Histogram* lh, feat::Histogram* rh,
			int flagsim, vecf wghtVec);
		std::pair<vecf, bool> calculateEdgeSimilarity(feat::Edge* lh, feat::Edge* rh, vecf weightVec,
			int flagsim, int magbin, int dirbin);
		float calculateHashSimilarity(feat::Hash* lhand, feat::Hash* rhand, vecf weightVec);
	};

	class Comparator {
	public:
		Comparator(FeatureVector* source) : source(source) {};
		void beginMultiCompare(std::vector<FeatureVector*> destVec, WeightVector* wvec);
	private:
		FeatureVector* source;
		std::vector<Comparison> dest;
	};
	
}