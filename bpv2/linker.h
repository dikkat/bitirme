#pragma once
#include "dbop.h"
#include <algorithm>

static dbop::Database* lnkr_dbPtr;

namespace lnkr {
	void setDatabaseClass(dbop::Database dbObj);
	img::Image createImage(const string& dir, int flag);
	img::Image createImage(const img::Image& image);
	void addToMainTable(img::Image* image);

	img::Image setSourceImage(const string& dir, int flag);
	img::Image setSourceImage(const img::Image& src);
	img::Image setDestinationImage(const string& dir, int flag);
	img::Image setDestinationImage(const img::Image& src);
	feat::Histogram* setHistogram(img::Image* image_ptr, int flag, int fb, int sb, int tb);
	feat::Edge::Canny* setEdgeCanny(img::Image* image_ptr, float gauss, float sigma, float thigh, float tlow, const cv::Mat& kernelx, const cv::Mat& kernely);
	feat::Edge* setEdge(img::Image* image_ptr, int flag, feat::Edge::Canny* edc = nullptr);
	feat::Corner::Harris* setCornerHarris(img::Image* image_ptr, float radius, float squareSize, float sigmai, float sigmad,
		float alpha, cv::Mat kernelx, cv::Mat kernely);
	feat::Corner* setCorner(img::Image* image_ptr, feat::Corner::Harris cdh, int flag, int numberofScales, float scaleRatio);
	void setIcon(img::Image* image_ptr);
	void setSimilarity(iop::Comparison* cmp_ptr);
	void setWeightVector(iop::WeightVector* wvec_ptr);

	float getSimilarity(img::Image* lhand, img::Image* rhand);
	std::vector<string> getImageDirs();
	string getImageDir(const string& hash);
	iop::Comparison getRawComparison(const string& hash, bool source);
	iop::WeightVector getWeightVector();
	std::vector<img::Image> getImageTable();
	std::vector<iop::Comparison> getSimilarityTable();

	void deleteImage(img::Image* image_ptr);
	void deleteFromSimAndWV();

	void insertDirectoryToDB(diriter dir, int max);
}