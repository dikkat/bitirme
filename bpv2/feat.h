#pragma once
#include <bitset>
#include <cmath>
#include "xxhash.h"
#include "sim.h"

enum hist_flag { HIST_BGR, HIST_HSV, HIST_GRAY };
enum edge_flag { EDGE_SOBEL, EDGE_PREWT, EDGE_ROBRT, EDGE_CANNY, EDGE_DRCHE };
enum corner_flag { CORNER_HARRIS, CORNER_HARLAP };

namespace feat {

	extern cv::Mat sobelX;
	extern cv::Mat sobelY;
	extern cv::Mat prewittX;
	extern cv::Mat prewittY;
	extern cv::Mat robertX;
	extern cv::Mat robertY;

	class Hash {
	public:
		static int compareImageHash(cv::Mat const lefthand, cv::Mat const righthand);
		static XXH64_hash_t hash_xxHash(cv::Mat const inputMat);
		static XXH64_hash_t setHash(std::vector<cv::Mat>* matVec = nullptr, std::vector<float>* floatVec = nullptr);
		static XXH64_hash_t setHash(std::vector<std::string> strVec);
	private:
		static std::bitset<64> imageHashing_dHash(cv::Mat const imageMat);
		static std::bitset<64> imageHashing_pHash(cv::Mat const imageMat);
	};

	class Histogram {
	public:
		Histogram(cv::Mat imageMat, int fb = 10, int sb = 4, int tb = 4, int flag = -1);
		cv::Mat getHistogramMat();
		cv::Mat getNormalizedHistogramMat();
		int* getBin();
		std::vector<float> getVariablesFloat();
		XXH64_hash_t getHash();
	private:
		XXH64_hash_t histHash;
		cv::Mat sourceMat;
		cv::Mat histMat, nhistMat;
		int fbin, sbin, tbin, histFlag;
		std::vector<cv::Mat> histBGR; //[0] BLUE, [1] GREEN, [2] RED
		std::vector<cv::Mat> nhistBGR;
		cv::Mat histogramHSVCalculation(cv::Mat sourceMat);
		cv::Mat histogramBGRCalculation(cv::Mat sourceMat);
		cv::Mat histogramGRAYCalculation(cv::Mat sourceMat);
		cv::Mat normalizeMat(cv::Mat sourceMat, float alpha, float beta);
		std::vector<cv::Mat> histogramBGRSeparateCalculation(cv::Mat sourceMat);
		cv::Mat createHistogramDisplayImage(std::vector<cv::Mat> bgrhist, int hist_w, int hist_h);
	};

	class Edge {
	public:
		class Canny {
		public:
			Canny(float kernelSize = 31, float sigm = 1.4, float thgh = 0.13,
				float tlw = 0.075, cv::Mat kerx = feat::prewittX, cv::Mat kery = feat::prewittY) :
				gaussKernelSize(kernelSize), sigma(sigm), thigh(thgh), tlow(tlw), kernelx(kerx), kernely(kery) {
				setHash();
			}
			cv::Mat edgeDetectionCanny(cv::Mat const imageMat);
			//void setVariables(std::string varName, float fltVal = 0, cv::Mat matVal = cv::Mat());
			std::vector<float> getVariablesFloat();
			std::vector<cv::Mat> getVariablesMat();
			XXH64_hash_t getHash();
		private:
			friend class Edge;
			XXH64_hash_t Hash;
			Edge* parent;
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
		Edge(cv::Mat imageMat, int flag, feat::Edge::Canny* edc = nullptr);
		int getEdgeFlag();
		std::vector<XXH64_hash_t> getHashVariables();
		static std::vector<cv::Mat> calculateEdgeGradientMagnitudeDirection(cv::Mat const kx, cv::Mat const ky, cv::Mat const imat);
	private:
		XXH64_hash_t edgeHash;
		XXH64_hash_t* edcHash = nullptr;
		Canny* child = nullptr;
		int edgeFlag;
		cv::Mat sourceMat;
		Canny* getCannyPtr();
		cv::Mat edgeDetectionSobel(cv::Mat const imageMat);
		cv::Mat edgeDetectionPrewitt(cv::Mat const imageMat);
		cv::Mat edgeDetectionRobertsCross(cv::Mat const imageMat);
		cv::Mat edgeDetectionDeriche(cv::Mat const imageMat, float alpha);
		cv::Mat commonOperationsSPR(cv::Mat const kernelx, cv::Mat const kernely, cv::Mat const imat);
	};

	class Corner {
	public:
		//NEED FURTHER EXPLORATION, IF I HAVE ENOUGH TIME I WOULD LIKE TO ADD HARRIS AFFINE DETECTOR AND BRISK DETECTOR	
		class Harris {
		public:
			Harris() : radius(3), squareSize(3), sigmai(1.4), sigmad(0.9),
				alpha(0.04), kernelx(feat::prewittX), kernely(feat::prewittY) {
				setHash();
			}
			void setVariables(std::string varName, float fltVal = 0, cv::Mat* matVal = nullptr);
			std::vector<float> getVariablesFloat();
			std::vector<cv::Mat> getVariablesMat();
			XXH64_hash_t getVariableHash();
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
		Corner(cv::Mat imageMat, int flag, int numofScales = 3, float scaleRat = 0, feat::Corner::Harris* cdh = nullptr);
		static cv::Mat paintPointsOverImage(cv::Mat const imageMat, cv::Mat const pointMat, bool gray = true, float numOfPoints = 100, float radius = 2, float thickness = 2, cv::Scalar pointColor = { 255,0,255 });
		static cv::Mat cornerDetectionHarrisLaplace(cv::Mat imageMat, float n = 3, float scaleRatio = 0);
		static void localMaxima(cv::Mat src, cv::Mat& dst, int squareSize, float threshold);
	private:
		XXH64_hash_t cornerHash;
		XXH64_hash_t* cdhHash = nullptr;
		Harris* child = nullptr;
		int edgeFlag;
		cv::Mat sourceMat;
	};	
}