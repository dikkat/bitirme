#pragma once
#include <bitset>
#include <cmath>
#include "xxhash.h"
#include "sim.h"

enum hist_flag { HIST_GRAY, HIST_BGR, HIST_HSV, HIST_DATA };
enum edge_flag { EDGE_SOBEL, EDGE_PREWT, EDGE_ROBRT, EDGE_CANNY, EDGE_DRCHE };
enum corner_flag { CORNER_HARRIS, CORNER_HARLAP };
enum kernel_flag {KERNEL_SOBEL, KERNEL_PREWT, KERNEL_ROBRT, KERNEL_ISOBEL};

namespace feat {
	extern cv::Mat sobelX;
	extern cv::Mat sobelY;
	extern cv::Mat prewittX;
	extern cv::Mat prewittY;
	extern cv::Mat robertX;
	extern cv::Mat robertY;
	extern cv::Mat isobelX;
	extern cv::Mat isobelY;

	class Feature {
	public:
		virtual bool skeleton() = 0;
		virtual bool empty() = 0;
	};

	class Gradient;

	class Hash : public Feature {
	public:
		Hash() {}
		//10 FOR D, 01 FOR P, 11 FOR BOTH
		Hash(cv::Mat const sourceMat, std::pair<bool, bool> selectHash = std::make_pair(true, true));
		std::pair<std::bitset<64>, std::bitset<64>> getHashVariables();
		std::pair<bool, bool> getSelectHash();
		static XXH64_hash_t hash_xxHash(cv::Mat const inputMat);
		static XXH64_hash_t setHash(std::vector<cv::Mat>* matVec = nullptr, vecf* floatVec = nullptr);
		static XXH64_hash_t setHash(std::vector<string> strVec);
		bool skeleton() override;
		bool empty() override;
	private:
		cv::Mat sourceMat;
		std::bitset<64> dHash = NULL;
		std::bitset<64> pHash = NULL;
		std::bitset<64> imageHashing_dHash(cv::Mat const imageMat);
		std::bitset<64> imageHashing_pHash(cv::Mat const imageMat);
		std::pair<bool, bool> selectHash;
	};

	class Histogram : public Feature {
	public:
		Histogram(cv::Mat imageMat, int flag = HIST_BGR, int fb = 10, int sb = 4, int tb = 4);
		Histogram() {};
		cv::Mat getHistogramMat();
		cv::Mat getNormalizedHistogramMat();
		int* getBin();
		//flag, fbin, sbin, tbin
		vecf getVariablesFloat();
		XXH64_hash_t getHash();
		//cv::Mat createHistogramDisplayImage(std::vector<cv::Mat> bgrhist, int hist_w, int hist_h); //OUTDATED
		//cv::Mat createHistogramDisplayImage(int hist_w, int hist_h); //OUTDATED
		bool skeleton() override;
		bool empty() override;
	private:
		XXH64_hash_t hash;
		cv::Mat sourceMat;
		cv::Mat histMat, nhistMat;
		int fbin, sbin, tbin, flag;
		std::vector<cv::Mat> histBGR; //[0] BLUE, [1] GREEN, [2] RED
		std::vector<cv::Mat> nhistBGR;
		std::pair<cv::Mat, cv::Mat> histogramCalculation(cv::Mat sourceMat, int hist_flag, std::vector<int> histSize,
			const float* histRange[]);
		/*cv::Mat histogramHSVCalculation(cv::Mat sourceMat);
		cv::Mat histogramBGRCalculation(cv::Mat sourceMat);
		cv::Mat histogramGRAYCalculation(cv::Mat sourceMat);*/
		cv::Mat normalizeHistMat(cv::Mat sourceMat, float alpha, float beta);
		std::vector<cv::Mat> histogramBGRSeparateCalculation(cv::Mat sourceMat);
	};

	class Gradient : public Feature {
	public:
		Gradient() {};
		Gradient(cv::Mat const imageMat, cv::Mat const kernelx, cv::Mat const kernely, float magbin = 10, float dirbin = 8);
		Gradient(cv::Mat magMat, cv::Mat dirMat, float magbin = 10, float dirbin = 8);
		std::pair<cv::Mat, cv::Mat> getGradientMats();
		std::pair<feat::Histogram, feat::Histogram> getGradientHists();
	private:
		friend class Edge;
		cv::Mat magMat;
		cv::Mat dirMat;
		feat::Histogram magHist;
		feat::Histogram dirHist;
		std::pair<feat::Histogram, feat::Histogram> calculateEdgeGradientHistograms(cv::Mat magMat,
			cv::Mat dirMat, int magbin, int dirbin);
		std::pair<cv::Mat, cv::Mat> calculateEdgeGradientMagnitudeDirection(cv::Mat const kx, cv::Mat const ky, cv::Mat const imat);
		bool skeleton() override { return false; }
		bool empty() override { return false; }
	};

	class Edge : public Feature {
	public:
		class Canny {
		public:
			Canny(float gaussKernelSize = 31, float sigma = 1.4, float thigh = 0.13, float tlow = 0.075, 
				cv::Mat kernelx = feat::prewittX, cv::Mat kernely = feat::prewittY) :
				gaussKernelSize(gaussKernelSize), sigma(sigma), thigh(thigh), tlow(tlow), kernelx(kernelx), kernely(kernely) {
				setHash();
			}
			cv::Mat edgeDetectionCanny(cv::Mat const imageMat);
			//void setVariables(string varName, float fltVal = 0, cv::Mat matVal = cv::Mat());
			vecf getVariablesFloat();
			std::vector<cv::Mat> getVariablesMat();
			XXH64_hash_t getHash();
		private:
			friend class Edge;
			XXH64_hash_t hash;
			Edge* parent = nullptr;
			cv::Mat sourceMat;
			float gaussKernelSize;
			float thigh;
			float tlow;
			float sigma;
			cv::Mat kernelx;
			cv::Mat kernely;
			void setHash();
			cv::Mat calculate(cv::Mat const imageMat);
			cv::Mat nonMaximumSuppression(cv::Mat& dirMat, cv::Mat& magMat);
			void performHysteresis(cv::Mat& resultMat, float weak, float strong);
			void doubleThreshold(cv::Mat& resultMat, cv::Mat const magMat, float max, float tlow, float thigh, float weakratio);
		}; //HAUSDORFF DISTANCE FOR EDGE COMPARISON ALSO MAYBE SIFT ALGORITHM
		Edge(cv::Mat sourceMat, int flag = EDGE_SOBEL, feat::Edge::Canny* edc = nullptr, float recommendedWidth = -1,
			int magbin = 10, int dirbin = 8);
		Edge(){}
		~Edge();
		Edge(const Edge& other);
		Edge& operator=(const Edge& other) {
			this->~Edge();
			if (other.child_edc) {
				this->child_edc = new Canny(*other.child_edc);
				this->child_edc->parent = this;
				this->edcHash = &this->child_edc->hash;
			}
			if (!other.sourceMat.empty()) {
				this->grad = new Gradient(*other.grad);
				this->sourceMat = other.sourceMat.clone();
				this->edgeMat = other.edgeMat.clone();
			}
			this->dirbin = other.dirbin;
			this->edgeFlag = other.edgeFlag;
			this->hash = other.hash;
			this->magbin = other.magbin;
			this->recommendedWidth = other.recommendedWidth;
			
			return *this;
		}
		int getEdgeFlag();
		std::vector<XXH64_hash_t> getHashVariables();
		cv::Mat getEdgeMat();
		Canny* getCannyPtr();
		Gradient* getGradientPtr();
		//recommendedWidth, magbin, dirbin
		std::vector<int> getComparisonValues();
		bool skeleton() override;
		bool empty() override;
	private:
		XXH64_hash_t hash;
		XXH64_hash_t* edcHash = nullptr;
		Canny* child_edc = nullptr;
		Gradient* grad = nullptr;
		int edgeFlag = -1;
		int magbin = 10;
		int dirbin = 8;
		int recommendedWidth;
		cv::Mat sourceMat;
		cv::Mat edgeMat;
		cv::Mat edgeDetectionSobel(cv::Mat const imageMat);
		cv::Mat edgeDetectionPrewitt(cv::Mat const imageMat);
		cv::Mat edgeDetectionRobertsCross(cv::Mat const imageMat);
		cv::Mat edgeDetectionDeriche(cv::Mat const imageMat, float alpha);
		cv::Mat commonOperationsSPR(cv::Mat const kernelx, cv::Mat const kernely, cv::Mat const imat);
	};

	class Corner : public Feature {
	public:
		//NEED FURTHER EXPLORATION, IF I HAVE ENOUGH TIME I WOULD LIKE TO ADD HARRIS AFFINE DETECTOR AND BRISK DETECTOR	
		class Harris {
		public:
			Harris(float radius = 3, float squareSize = 3, float sigmai = 1.4, float sigmad = 0.9, float alpha = 0.04,
				cv::Mat kernelx = feat::prewittX, cv::Mat kernely = feat::prewittY) : 
				radius(radius), squareSize(squareSize), sigmai(sigmai), sigmad(sigmad), alpha(alpha),
				kernelx(kernelx), kernely(kernely) {
				setHash();
			}
			void setVariables(string varName, float fltVal = 0, cv::Mat* matVal = nullptr);
			vecf getVariablesFloat();
			std::vector<cv::Mat> getVariablesMat();
			XXH64_hash_t getHash();
			cv::Mat cornerDetectionHarris(cv::Mat const imageMat);
		private:
			friend class Corner;
			Corner* parent;
			XXH64_hash_t hash;
			float radius;
			float squareSize;
			float sigmai;
			float sigmad;
			float alpha;
			cv::Mat sourceMat;
			cv::Mat kernelx;
			cv::Mat kernely;
			void setHash();
			cv::Mat calculate(cv::Mat const imageMat);
		};
		Corner(cv::Mat imageMat, feat::Corner::Harris* cdh, int flag, int numberofScales = 3, float scaleRat = 0);
		Corner() {}
		Corner(const Corner& other);
		~Corner();
		Corner& operator=(const Corner& other) {
			this->~Corner();
			if (other.child) {
				this->child = new Harris(*other.child);
				this->child->parent = this;
				this->cdhHash = &this->child->hash;
			}
			this->hash = other.hash;
			this->cornerFlag = other.cornerFlag;
			this->numofScales = other.numofScales;
			this->scaleRatio = other.scaleRatio;
			this->sourceMat = other.sourceMat.clone();
			this->cornerMat = other.cornerMat.clone();
			this->cornerMarkedMat = other.cornerMarkedMat.clone();
			
			return *this;
		}
		std::vector<int> getIntVariables();
		float getScaleRatio();
		std::vector<XXH64_hash_t> getHashVariables();
		cv::Mat getCornerMat();
		cv::Mat getCornerMarkedMat(bool gray = false, float numOfPoints = 1000, float radius = 2, float thickness = 2,
			cv::Scalar pointColor = { 255,0,255 });
		static cv::Mat paintPointsOverImage(cv::Mat const imageMat, cv::Mat const pointMat, bool gray = true,
			float numOfPoints = 100, float radius = 2, float thickness = 2, cv::Scalar pointColor = { 255,0,255 });
		static cv::Mat cornerDetectionHarrisLaplace(cv::Mat imageMat, feat::Corner::Harris* cdh, float n = 3, float scaleRatio = 0);
		static void localMaxima(cv::Mat src, cv::Mat& dst, int squareSize, float threshold);
		bool skeleton() override { return false; }
		bool empty() override { return false; }
	private:
		XXH64_hash_t hash;
		XXH64_hash_t* cdhHash = nullptr;
		Harris* child = nullptr;
		int cornerFlag;
		int numofScales;
		float scaleRatio;
		cv::Mat sourceMat;
		cv::Mat cornerMat;
		cv::Mat cornerMarkedMat;
	};
}