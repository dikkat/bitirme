#pragma once
#include "image.h"

enum hist_flag { HIST_BGR, HIST_HSV, HIST_GRAY };
enum edge_flag { EDGE_SOBEL, EDGE_PREWT, EDGE_ROBRT, EDGE_CANNY, EDGE_DRCHE };

namespace feat {

	extern cv::Mat sobelX;
	extern cv::Mat sobelY;
	extern cv::Mat prewittX;
	extern cv::Mat prewittY;
	extern cv::Mat robertX;
	extern cv::Mat robertY;

	class Feature {
	public:
		img::Image* getSourceImg();
		cv::Mat getSourceMat();
	protected:
		img::Image* src_img;
		cv::Mat srcMat;
	};

	class Histogram : public Feature {
	public:
		Histogram(img::Image* srimg, int fb, int sb, int tb, int flag = 0);
		Histogram(cv::Mat mat, int fb, int sb, int tb, int flag = 0);
		Histogram() {}
		cv::Mat getHistogramMat();
		cv::Mat getNormalizedHistogramMat();
		int* getBin();
		void setHistogramDisplayImage(int w, int h);
	private:
		cv::Mat histMat, nhistMat;
		int fbin, sbin, tbin;
		img::Image* hist_dsp_img;
		std::vector<cv::Mat> histBGR; //[0] BLUE, [1] GREEN, [2] RED
		std::vector<cv::Mat> nhistBGR;
		cv::Mat histogramHSVCalculation(cv::Mat sourcemat);
		cv::Mat histogramBGRCalculation(cv::Mat sourcemat);
		cv::Mat histogramGRAYCalculation(cv::Mat sourcemat);
		cv::Mat normalizeMat(cv::Mat sourcemat, float alpha, float beta);
		std::vector<cv::Mat> histogramBGRSeparateCalculation(cv::Mat sourcemat);
		img::Image createHistogramDisplayImage(std::vector<cv::Mat> bgrhist, int hist_w, int hist_h);
	};

	class Edge : public Feature {
	public:
		Edge(img::Image* srimg, int flag);

		static cv::Mat edgeDetectionSobel(cv::Mat const imageMat);
		static cv::Mat edgeDetectionPrewitt(cv::Mat const imageMat);
		static cv::Mat edgeDetectionRobertsCross(cv::Mat const imageMat);
		static cv::Mat edgeDetectionDeriche(cv::Mat const imageMat, float alpha);
		static cv::Mat commonOperationsSPR(cv::Mat const kernelx, cv::Mat const kernely, cv::Mat const imat);

		static std::vector<cv::Mat> calculateEdgeGradientMagnitudeDirection(cv::Mat const kx, cv::Mat const ky, cv::Mat const imat);

		class EdgeDetectorCanny {
		public:
			EdgeDetectorCanny() : gaussKernelSize(31), sigma(1.4), mu(0), thigh(0.13), tlow(0.075), weakratio(0.09),
				kernelx(prewittX), kernely(prewittY) {}

			static cv::Mat edgeDetectionCanny(cv::Mat const imageMat, feat::Edge::EdgeDetectorCanny edcOperator = feat::Edge::EdgeDetectorCanny());

			float gaussKernelSize;
			float thigh;
			float tlow;
			float sigma;
			float mu;
			float weakratio;
			cv::Mat kernelx;
			cv::Mat kernely;

		private:			
			cv::Mat nonMaximumSuppression(cv::Mat& dirMat, cv::Mat& magMat);
			void performHysteresis(cv::Mat& resultMat, float weak, float strong);
			void doubleThreshold(cv::Mat& resultMat, cv::Mat const magMat, float max, float tlow, float thigh, float weakratio);
		}; //HAUSDORFF DISTANCE FOR EDGE COMPARISON ALSO MAYBE SIFT ALGORITHM
	};

	class Corner : public Feature {
	public:
		static cv::Mat paintPointsOverImage(cv::Mat const image, cv::Mat const pointMat, bool gray = true, float numOfPoints = 100, float radius = 2, float thickness = 2, cv::Scalar pointColor = { 255,0,255 });
		static cv::Mat cornerDetectionHarrisLaplace(cv::Mat image, float scaleRatio = 1, float n = 3);
		static void localMaxima(cv::Mat src, cv::Mat& dst, int squareSize, float threshold);
		class cornerDetectorHarris {
		public:
			cornerDetectorHarris() : numOfPoints(100), radius(3), squareSize(15), threshold(254), 
				sigma(1.4), alpha(0.04), kernelx(feat::sobelX), kernely(feat::sobelY) {}
			static cv::Mat cornerDetectionHarris(cv::Mat const image, feat::Corner::cornerDetectorHarris cdhOperator = feat::Corner::cornerDetectorHarris());
			cv::Mat const kernelx;
			cv::Mat const kernely;
			float numOfPoints;
			float radius;
			float squareSize;
			float threshold;
			float sigma;
			float alpha;
		private:
		};
	
		
		
			
	};
	
}