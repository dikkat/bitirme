#pragma once
#include <bitset>
#include <cmath>
#include "xxhash.h"
#include "sim.h"

enum hist_flag { HIST_GRAY, HIST_BGR, HIST_HSV };
enum edge_flag { EDGE_SOBEL, EDGE_PREWT, EDGE_ROBRT, EDGE_CANNY, EDGE_DRCHE };
enum corner_flag { CORNER_HARRIS, CORNER_HARLAP };
enum kernel_flag {KERNEL_SOBEL, KERNEL_PREWT, KERNEL_ROBRT};

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
		static std::bitset<64> imageHashing_dHash(cv::Mat const imageMat);
		static std::bitset<64> imageHashing_pHash(cv::Mat const imageMat);
	};

	class Histogram {
	public:
		Histogram(cv::Mat imageMat, int flag = -1, int fb = 10, int sb = 4, int tb = 4);
		cv::Mat getHistogramMat();
		cv::Mat getNormalizedHistogramMat();
		int* getBin();
		std::vector<float> getVariablesFloat();
		XXH64_hash_t getHash();
		cv::Mat createHistogramDisplayImage(std::vector<cv::Mat> bgrhist, int hist_w, int hist_h);
		cv::Mat createHistogramDisplayImage(int hist_w, int hist_h);
	private:
		XXH64_hash_t hash;
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
	};

	class Edge {
	public:
		class Canny {
		public:
			Canny(float gaussKernelSize = 31, float sigma = 1.4, float thigh = 0.13, float tlow = 0.075, 
				cv::Mat kernelx = feat::prewittX, cv::Mat kernely = feat::prewittY) :
				gaussKernelSize(gaussKernelSize), sigma(sigma), thigh(thigh), tlow(tlow), kernelx(kernelx), kernely(kernely) {
				setHash();
			}
			cv::Mat edgeDetectionCanny(cv::Mat const imageMat);
			//void setVariables(std::string varName, float fltVal = 0, cv::Mat matVal = cv::Mat());
			std::vector<float> getVariablesFloat();
			std::vector<cv::Mat> getVariablesMat();
			XXH64_hash_t getHash();
		private:
			friend class Edge;
			XXH64_hash_t hash;
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
		Edge(){}
		int getEdgeFlag();
		std::vector<XXH64_hash_t> getHashVariables();
		cv::Mat getEdgeMat();
		static std::vector<cv::Mat> calculateEdgeGradientMagnitudeDirection(cv::Mat const kx, cv::Mat const ky, cv::Mat const imat);
	private:
		XXH64_hash_t hash;
		XXH64_hash_t* edcHash = nullptr;
		Canny* child_edc = nullptr;
		int edgeFlag;
		cv::Mat edgeMat;
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
			Harris(float radius = 3, float squareSize = 3, float sigmai = 1.4, float sigmad = 0.9, float alpha = 0.04,
				cv::Mat kernelx = feat::prewittX, cv::Mat kernely = feat::prewittY) : 
				radius(radius), squareSize(squareSize), sigmai(sigmai), sigmad(sigmad), alpha(alpha),
				kernelx(kernelx), kernely(kernely) {
				setHash();
			}
			void setVariables(std::string varName, float fltVal = 0, cv::Mat* matVal = nullptr);
			std::vector<float> getVariablesFloat();
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