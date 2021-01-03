#pragma once
#include "image.h"
#include "sim.h"
#include "feat.h"
#include <QList>
#include <mutex>

enum cmpr_flag { SIM_COSSIM, SIM_MANDIST, SIM_EUCDIST, SIM_MINKDIST, SIM_JACSIM, SIM_HISINTR, SIM_CROCOR, SIM_CSQDIST };

namespace img {
	class Image;
}

namespace iop {
	bool checkVectorEmpty(vecf operand);
	std::pair<float, bool> calculateVectorSimilarity(vecf const lh, vecf const rh, int flag);
	int getMinkowskiOrder();
	void setMinkowskiOrder(int value);
	extern int minkorder;

	class FeatureVector {
	public:
		FeatureVector() {}
		FeatureVector(bool def_fv);
		FeatureVector(img::Image* image, feat::Edge* edge = nullptr, feat::Histogram* hist_gray = nullptr,
			feat::Histogram* hist_bgr = nullptr, feat::Histogram* hist_hsv = nullptr, feat::Hash* perc_hash = nullptr);
		img::Image* image;
		std::vector<bool>* enabler;
		feat::Edge* edge;
		feat::Histogram *hist_gray, *hist_bgr, *hist_hsv;
		feat::Hash* perc_hash;
	};
	
	class WeightVector {
	public:
		WeightVector() {}
		WeightVector(bool equal) : WeightVector(&equalf, &equalf, &equalf, &equalf, &equalf) {}
		WeightVector(std::vector<vecf> wvv_total);
		WeightVector(vecf* wv_grad = nullptr, float* w_hgray = nullptr, vecf* wv_hbgr = nullptr, 
			vecf* wv_hhsv = nullptr, vecf* wv_hash = nullptr);
		WeightVector(float* w_grad = nullptr, float* w_hgray = nullptr, float* w_hbgr = nullptr,
			float* w_hhsv = nullptr, float* w_hash = nullptr);
		float equalf = 0.2;
		float w_hgray = 0;
		vecf wv_grad, wv_hbgr, wv_hhsv, wv_hash;
		std::vector<vecf> wvv_total;
	};

	class Comparison {
	public:
		Comparison(FeatureVector* source, FeatureVector* rhand, WeightVector* wvec);
		std::pair<string, string> getDirValues();
		float getEuclideanDistance();
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
		Comparator() {}
		std::vector<Comparison> beginMultiCompare(FeatureVector* source, std::vector<std::vector<string>> destVec,
			WeightVector* wvec, int cmpNum);
		std::vector<Comparison> getComparisonVector(bool sorted);
	private:
		FeatureVector* source;
		std::vector<Comparison> dest;
	};
	
}