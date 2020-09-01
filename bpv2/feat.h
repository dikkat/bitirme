#pragma once
#include "image.h"
#include <bitset>
#include "xxhash.h"

enum hist_flag { HIST_BGR, HIST_HSV, HIST_GRAY };
enum edge_flag { EDGE_SOBEL, EDGE_PREWT, EDGE_ROBRT, EDGE_CANNY, EDGE_DRCHE };

namespace feat {

	extern cv::Mat sobelX;
	extern cv::Mat sobelY;
	extern cv::Mat prewittX;
	extern cv::Mat prewittY;
	extern cv::Mat robertX;
	extern cv::Mat robertY;

	class Hash {
	public:
		static std::bitset<64> imageHashing_dHash(cv::Mat const imageMat);
		static std::bitset<64> imageHashing_pHash(cv::Mat const imageMat);
		static int compareImageHash(cv::Mat const lefthand, cv::Mat const righthand);
		static XXH64_hash_t hash_xxHash(cv::Mat const inputMat);
	};

	class Histogram {
	public:
		Histogram(cv::Mat imageMat, int fb, int sb, int tb, int flag = 0);
		Histogram() {}
		cv::Mat getHistogramMat();
		cv::Mat getNormalizedHistogramMat();
		int* getBin();
	private:
		cv::Mat histMat, nhistMat;
		int fbin, sbin, tbin;
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
		Edge() {};
		static cv::Mat edgeDetection(cv::Mat const imageMat, int flag, Edge edgeOper = Edge());

		static std::vector<cv::Mat> calculateEdgeGradientMagnitudeDirection(cv::Mat const kx, cv::Mat const ky, cv::Mat const imat);
		
		class EdgeDetectorCanny {
		public:
			EdgeDetectorCanny() : gaussKernelSize(31), sigma(1.4), thigh(0.13), tlow(0.075),
				kernelx(prewittX), kernely(prewittY) {
				setHash();
			}
			void setVariables(std::string varName, float fltVal = 0, cv::Mat matVal = cv::Mat());
			void setHash();
			static cv::Mat edgeDetectionCanny(cv::Mat const imageMat, feat::Edge::EdgeDetectorCanny edcOperator = feat::Edge::EdgeDetectorCanny());
			XXH64_hash_t edcHash;
		private:
			float gaussKernelSize;
			float thigh;
			float tlow;
			float sigma;
			cv::Mat kernelx;
			cv::Mat kernely;
			cv::Mat nonMaximumSuppression(cv::Mat& dirMat, cv::Mat& magMat);
			void performHysteresis(cv::Mat& resultMat, float weak, float strong);
			void doubleThreshold(cv::Mat& resultMat, cv::Mat const magMat, float max, float tlow, float thigh, float weakratio);
		}; //HAUSDORFF DISTANCE FOR EDGE COMPARISON ALSO MAYBE SIFT ALGORITHM
	private:
		cv::Mat edgeDetectionSobel(cv::Mat const imageMat);
		cv::Mat edgeDetectionPrewitt(cv::Mat const imageMat);
		cv::Mat edgeDetectionRobertsCross(cv::Mat const imageMat);
		cv::Mat edgeDetectionDeriche(cv::Mat const imageMat, float alpha);
		cv::Mat commonOperationsSPR(cv::Mat const kernelx, cv::Mat const kernely, cv::Mat const imat);
	};

	class Corner {
	public:
		static cv::Mat paintPointsOverImage(cv::Mat const imageMat, cv::Mat const pointMat, bool gray = true, float numOfPoints = 100, float radius = 2, float thickness = 2, cv::Scalar pointColor = { 255,0,255 });
		//NEED FURTHER EXPLORATION, IF I HAVE ENOUGH TIME I WOULD LIKE TO ADD HARRIS AFFINE DETECTOR AND BRISK DETECTOR
		static cv::Mat cornerDetectionHarrisLaplace(cv::Mat imageMat, float n = 3, float scaleRatio = 0);
		static void localMaxima(cv::Mat src, cv::Mat& dst, int squareSize, float threshold);

		class cornerDetectorHarris {
		public:
			cornerDetectorHarris() : radius(3), squareSize(3), sigmai(1.4), sigmad(0.9), 
				alpha(0.04), kernelx(feat::prewittX), kernely(feat::prewittY) {
				setHash();
			}
			void setVariables(std::string varName, float fltVal = 0, cv::Mat matVal = cv::Mat());
			void setHash();
			static cv::Mat cornerDetectionHarris(cv::Mat const imageMat, feat::Corner::cornerDetectorHarris cdhOperator = feat::Corner::cornerDetectorHarris());
			XXH64_hash_t cdhHash;
		private:
			cv::Mat kernelx;
			cv::Mat kernely;
			float radius;
			float squareSize;
			float sigmai;
			float sigmad;
			float alpha;
		};
	};	
}