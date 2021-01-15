#pragma once
#include "feat.h"
#include "image.h"
#include "dbop.h"
#include "iop.h"
#include <algorithm>

static dbop::Database* lnkr_dbPtr;

namespace lnkr {
	void setDatabaseClass(dbop::Database dbObj);
	img::Image createImage(string dir, int flag);
	void addToMainTable(img::Image* image);

	img::Image setSourceImage(string dir, int flag);
	img::Image setSourceImage(img::Image src);
	img::Image setDestinationImage(string dir, int flag);
	img::Image setDestinationImage(img::Image src);
	feat::Histogram* setHistogram(img::Image* image_ptr, int flag, int fb, int sb, int tb);
	feat::Edge::Canny* setEdgeCanny(img::Image* image_ptr, float gauss, float sigma, float thigh, float tlow, cv::Mat kernelx, cv::Mat kernely);
	feat::Edge* setEdge(img::Image* image_ptr, int flag, feat::Edge::Canny* edc = nullptr);
	feat::Corner::Harris* setCornerHarris(img::Image* image_ptr, float radius, float squareSize, float sigmai, float sigmad,
		float alpha, cv::Mat kernelx, cv::Mat kernely);
	feat::Corner* setCorner(img::Image* image_ptr, feat::Corner::Harris cdh, int flag, int numberofScales, float scaleRatio);
	void setIcon(img::Image* image_ptr);
	void setSimilarity(iop::Comparison* cmp_ptr);
	void setWeightVector(iop::WeightVector* wvec_ptr);

	feat::Edge* getImageEdge(img::Image* image_ptr, XXH64_hash_t edgeHash);
	feat::Edge::Canny* getEdgeCanny(img::Image* image_ptr, XXH64_hash_t cannyHash);
	feat::Corner* getImageCorner(img::Image* image_ptr, XXH64_hash_t cornerHash);
	feat::Corner::Harris* getCornerHarris(img::Image* image_ptr, XXH64_hash_t harrisHash);
	feat::Histogram* getImageHist(img::Image* image_ptr, XXH64_hash_t histHash);
	float getSimilarity(img::Image* lhand, img::Image* rhand);
	std::vector<string> getImageDirs();
	string getImageDir(string hash);
	iop::Comparison getRawComparison(string hash, bool source);
	iop::WeightVector getWeightVector();
	void deleteImage(img::Image* image_ptr);
	void deleteFromSimAndWV();

	void insertDirectoryToDB(diriter dir, int max);
}