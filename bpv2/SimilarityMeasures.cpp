#include "SimilarityMeasures.h"

bool sim::vectorSizeEqualityCheck(std::vector<float> lefthand, std::vector<float> righthand) {
	if (lefthand.size() != righthand.size())
		throw std::exception("Two vectors should be same size");
	if (lefthand.size() == 0 || righthand.size() == 0)
		throw std::exception("Vector size can't be zero.");
	return true;
}

float sim::similarityCosine(std::vector<float> lefthand, std::vector<float> righthand) {
	vectorSizeEqualityCheck(lefthand, righthand);

	std::vector<float> prdvec, lsqvec, rsqvec;
	float vecoperator = 0;

	for (int i = 0; i < lefthand.size(); i++) {
		vecoperator = lefthand[i] * righthand[i];
		prdvec.push_back(vecoperator);

		vecoperator = std::pow(lefthand[i], 2);
		lsqvec.push_back(vecoperator);

		vecoperator = std::pow(righthand[i], 2);
		rsqvec.push_back(vecoperator);
	}

	float numerator = sumOfVectorMembers(prdvec);
	float denominator = std::sqrt(sumOfVectorMembers(lsqvec)) * std::sqrt(sumOfVectorMembers(rsqvec));
	float cosoperator = numerator / denominator;

	return cosoperator;
}

float sim::similarityJaccard(std::vector<float> lefthand, std::vector<float> righthand) {
	vectorSizeEqualityCheck(lefthand, righthand);

	float numerator = 0;
	float denominator = 0;

	for (int i = 0; i < lefthand.size(); i++) {
		numerator += MIN(lefthand[i], righthand[i]);
		denominator += MAX(lefthand[i], righthand[i]);
	}
	
	float jacoperator = numerator / denominator;

	return jacoperator;
}

float sim::distanceManhattan(std::vector<float> lefthand, std::vector<float> righthand) {
	vectorSizeEqualityCheck(lefthand, righthand);
	
	float dstoperator = 0;
	for (int i = 0; i < lefthand.size(); i++) {
		dstoperator += abs(lefthand[i] - righthand[i]);
	}

	return dstoperator;
}

float sim::distanceEuclidean(std::vector<float> lefthand, std::vector<float> righthand) {
	vectorSizeEqualityCheck(lefthand, righthand);

	float dstoperator = 0;
	for (int i = 0; i < lefthand.size(); i++) {
		dstoperator += pow(lefthand[i] - righthand[i], 2);
	}

	return sqrt(dstoperator);
}

float sim::distanceMinkowski(std::vector<float> lefthand, std::vector<float> righthand, int order) {
	vectorSizeEqualityCheck(lefthand, righthand);

	float dstoperator = 0;
	for (int i = 0; i < lefthand.size(); i++) {
		dstoperator += pow(abs(lefthand[i] - righthand[i]), order);
	}
	dstoperator = pow(dstoperator, (float) 1 / order);
	return dstoperator;
}

float sim::distanceChiSquared(std::vector<float> lefthand, std::vector<float> righthand) {
	vectorSizeEqualityCheck(lefthand, righthand);

	float numerator = 0;
	float denominator = 0;
	float dcsoperator = 0;
	for (int i = 0; i < lefthand.size(); i++) {
		numerator = pow(lefthand[i] - righthand[i], 2);
		denominator = lefthand[i];
		if (denominator == 0)
			continue;
		dcsoperator += numerator / denominator;
	}
	
	return dcsoperator;
}

float sim::histogramIntersection(std::vector<float> lefthand, std::vector<float> righthand) {
	vectorSizeEqualityCheck(lefthand, righthand);

	float numerator = 0, denominator;

	for (int i = 0; i < lefthand.size(); i++) {
		numerator += MIN(lefthand[i], righthand[i]);
	}

	denominator = sumOfVectorMembers(righthand);
	float hintroperator = numerator / denominator;
	return hintroperator;
}

float sim::crossCorrelation(std::vector<float> lefthand, std::vector<float> righthand) {
	vectorSizeEqualityCheck(lefthand, righthand);

	float numerator = 0, meanlh, meanrh, devoperatorlh, devoperatorrh, crcooperator, denominatorlh = 0, denominatorrh = 0;

	meanlh = sumOfVectorMembers(lefthand) / lefthand.size();
	meanrh = sumOfVectorMembers(righthand) / righthand.size();

	for (int i = 0; i < lefthand.size(); i++) {
		devoperatorlh = lefthand[i] - meanlh;
		devoperatorrh = righthand[i] - meanrh;
		numerator += (devoperatorlh) * (devoperatorrh);
		denominatorlh += pow(devoperatorlh, 2);
		denominatorrh += pow(devoperatorrh, 2);
	}

	crcooperator = numerator / sqrt(denominatorlh * denominatorrh);

	return crcooperator;
}

cv::Mat sim::edgeDetectionCanny(cv::Mat const imageMat, sim::edgeDetectorCanny edcOperator) { //Computer Vision, Mar 2000, Alg 24
	cv::Mat cannyoper;
	if (imageMat.channels() == 3)
		cv::cvtColor(imageMat, cannyoper, cv::COLOR_BGR2GRAY);
	else
		cannyoper = imageMat.clone();
	cannyoper = sim::filterGauss(cannyoper, edcOperator.kernelsize, edcOperator.sigma, edcOperator.mu);
	cv::Mat kernelx = (cv::Mat_<float>(3, 3) << 1, 0, -1, 2, 0, -2, 1, 0, -1);
	cv::Mat kernely = (cv::Mat_<float>(3, 3) << 1, 2, 1, 0, 0, 0, -1, -2, -1);
	std::vector<cv::Mat> temp = sim::calculateEdgeGradientMagnitudeDirection(kernelx, kernely, cannyoper);
	cv::Mat magMat = std::ref(temp[0]);
	cv::Mat dirMat = std::ref(temp[1]);

	int posDel[4][2] = { {1,0},{1,1},{0,1},{-1,1} };
	int negDel[4][2] = { {-1,0},{-1,-1},{0,-1},{1,-1} };
	int direction;

	for (int i = 0; i < imageMat.rows; i++) {
		for (int j = 0; j < imageMat.cols; j++) {
			float dirOper = (dirMat.at<float>(i, j) + (M_PI / 8)) / (M_PI / 4);
			direction = roundf(dirOper);
			direction = direction % 4;
			if (magMat.at<float>(i, j) <= magMat.at<float>(i + posDel[direction][0], j + posDel[direction][1]))
				magMat.at<float>(i, j) = 0;
			else if(magMat.at<float>(i, j) <= magMat.at<float>(i + negDel[direction][0], j + negDel[direction][1]))
				magMat.at<float>(i, j) = 0;
		}
	}

	cv::Mat resultMat = cv::Mat::zeros(magMat.rows, magMat.cols, magMat.type());

	//http://www.tomgibara.com/computer-vision/CannyEdgeDetector.java
	std::function <void(int, int, int, int)> follow = [&resultMat, &magMat, &follow](int x1, int y1, int i1, int threshold) {
		int x0 = x1 == 0 ? x1 : x1 - 1;
		int x2 = x1 == magMat.rows - 1 ? x1 : x1 + 1;
		int y0 = y1 == 0 ? y1 : y1 - 1;
		int y2 = y1 == magMat.cols - 1 ? y1 : y1 + 1;

		resultMat.at<float>(i1) = magMat.at<float>(i1);
		for (int x = x0; x <= x2; x++) {
			for (int y = y0; y <= y2; y++) {
				int i2 = x + y * resultMat.cols;
				if ((y != y1 || x != x1)
					&& resultMat.at<float>(i2) == 0
					&& magMat.at<float>(i2) >= threshold) {
					follow(x, y, i2, threshold);
					return;
				}
			}
		}
	};

	auto performHysteresis = [&resultMat, &magMat, follow](int low, int high) {
		int offset = 0;
		for (int y = 0; y < resultMat.cols; y++) {
			for (int x = 0; x < resultMat.rows; x++) {
				if (resultMat.at<float>(offset) == 0 && magMat.at<float>(offset) >= high) {
					follow(x, y, offset, low);
				}
				offset++;
			}
		}
	};

	performHysteresis;
	return resultMat;
}

cv::Mat sim::edgeDetectionSobel(cv::Mat const imageMat) {
	cv::Mat kernelx = (cv::Mat_<float>(3, 3) << 1, 0, -1, 2, 0, -2, 1, 0, -1);
	cv::Mat kernely = (cv::Mat_<float>(3, 3) << 1, 2, 1, 0, 0, 0, -1, -2, -1);

	cv::Mat sobelg = commonOperationsSPR(kernelx, kernely, imageMat);
	return sobelg;
}

cv::Mat sim::edgeDetectionPrewitt(cv::Mat const imageMat) {
	cv::Mat kernelx = (cv::Mat_<float>(3, 3) << 1, 0, -1, 1, 0, -1, 1, 0, -1);
	cv::Mat kernely = (cv::Mat_<float>(3, 3) << 1, 1, 1, 0, 0, 0, -1, -1, -1);

	cv::Mat prewittg = commonOperationsSPR(kernelx, kernely, imageMat);
	return prewittg;
}

cv::Mat sim::edgeDetectionRobertsCross(cv::Mat const imageMat) {
	cv::Mat kernelx = (cv::Mat_<float>(2, 2) << 1, 0, 0, -1);
	cv::Mat kernely = (cv::Mat_<float>(2, 2) << 0, 1, -1, 0);

	cv::Mat robertsg = commonOperationsSPR(kernelx, kernely, imageMat);
	return robertsg;
}

cv::Mat sim::commonOperationsSPR(cv::Mat const kx, cv::Mat const ky, cv::Mat const imat) {
	cv::Mat kernelx = kx;
	cv::Mat kernely = ky;

	cv::Mat sprgx = convolution2D(imat, kernelx);
	cv::Mat sprgy = convolution2D(imat, kernely);

	std::vector<float> xde = matToVector<float>(sprgx);   //REMOVE THESE LATER AND FIND OUT WHY THESE HAVE TO BE HERE
	cv::Mat ahwell = vectorToMat(xde);						   //AND WHY REMOVING THEM CAUSES UNRESOLVED EXTERNAL

	cv::Mat sprg(sprgx.dims, sprgx.size, sprgx.type());
	for (int i = 0; i < sprg.rows; i++) {
		for (int j = 0; j < sprg.cols; j++)
			sprg.at<float>(i, j) = hypot(sprgx.at<float>(i, j), sprgy.at<float>(i, j));
	}
	return sprg;
}

std::vector<cv::Mat> sim::calculateEdgeGradientMagnitudeDirection(cv::Mat const kx, cv::Mat const ky, cv::Mat const imat) {
	cv::Mat kernelx = kx;
	cv::Mat kernely = ky;

	cv::Mat sprgx = convolution2D(imat, kernelx);
	cv::Mat sprgy = convolution2D(imat, kernely);

	cv::Mat sprgMag(sprgx.dims, sprgx.size, sprgx.type());
	cv::Mat sprgDir(sprgx.dims, sprgx.size, sprgx.type());

	for (int i = 0; i < sprgMag.rows; i++) {
		for (int j = 0; j < sprgMag.cols; j++)
			sprgMag.at<float>(i, j) = hypot(sprgx.at<float>(i, j), sprgy.at<float>(i, j));
	}

	for (int i = 0; i < sprgDir.rows; i++) {
		for (int j = 0; j < sprgDir.cols; j++)
			sprgDir.at<float>(i, j) = atan2f(sprgy.at<float>(i, j), sprgx.at<float>(i, j));
	}


	return std::vector<cv::Mat>{sprgMag, sprgDir};
}



template <typename T>
std::vector<T> sim::matToVector(cv::Mat operand) {   //https://stackoverflow.com/questions/62325615/opencv-data-of-two-mats-are-the-same-but-values-when-retrieved-with-matat-are
	std::vector<T> vecoper;
	uchar* pixelPtr = (uchar*)operand.data;
	int cn = operand.channels();
	cv::Scalar_<uchar> cnPixel;

	vecoper.push_back((T)operand.dims);
	for (int i = 0; i < (T)operand.dims; i++) {
		vecoper.push_back((T)operand.size[i]);
	}
	vecoper.push_back((T)operand.type());

	for (int i = 0; i < operand.total() * operand.elemSize(); i++) {
		vecoper.push_back((T)pixelPtr[i]);
	}

	return vecoper;
}

template <typename T>
cv::Mat sim::vectorToMat(std::vector<T> operand) { //PARALLELLISE
	int mdims, mtype;

	mdims = operand[0];
	int* msize = new int[mdims];
	int i = 0;
	for (i = 1; i < mdims + 1; i++) {
		msize[i - 1] = operand[i];
	}
	mtype = operand[i];
	i++;

	cv::Mat matoper(mdims, msize, mtype);
	delete[] msize;

	uchar* pixelPtr = (uchar*)matoper.data;
	float cnpixoper;
	for (int j = 0; j < matoper.total() * matoper.elemSize(); j++) {
		cnpixoper = operand[i + j];
		pixelPtr[j] = cnpixoper;
	}
	return matoper;
}

cv::Mat sim::convolution2D(cv::Mat const image, cv::Mat kernel) {  // CPU INTENSIVE FUNCTION -- PARALLELISE
	cv::Mat oper;
	if (image.channels() == 3)
		cv::cvtColor(image, oper, cv::COLOR_BGR2GRAY);
	else
		oper = image.clone();
	double kx = kernel.rows, ky = kernel.cols;

	cv::Mat invkernel = rotateMatrix180(kernel);

	int ix1 = image.rows, iy1 = image.cols;
	int ix2 = ix1 + ceil(kx / 2);
	int iy2 = iy1 + ceil(ky / 2);
	cv::Mat convmat = cv::Mat::zeros(ix2, iy2, CV_32FC1);
	std::cout << img::typeToString(oper.type());
	for (int i = 0; i < ix2; i++)
		for (int j = 0; j < iy2; j++){
			for (int m = 0; m < kx; m++)
				for (int n = 0; n < ky; n++) {
					if (i - floor(kx / 2) + m < 0 || j - floor(ky / 2) + n < 0 || i - floor(kx / 2) + m >= ix1 || j - floor(ky / 2) + n >= iy1)
						continue;
					else {
						uchar uop;
						float fop1;

						if (oper.elemSize() == 1) {
							uop = oper.at<uchar>(i - floor(kx / 2) + m, j - floor(ky / 2) + n);
							fop1 = (float)(uop);
						}

						else 
							fop1 = oper.at<float>(i - floor(kx / 2) + m, j - floor(ky / 2) + n);		

						float fop2 = invkernel.at<float>(m, n);
						convmat.at<float>(i, j) = convmat.at<float>(i, j) + fop1 * fop2;
					}
				}
			convmat.at<float>(i, j) += 1;
		}
	return convmat;
}

cv::Mat sim::rotateMatrix180(cv::Mat srcmat)
{
	if (srcmat.rows != srcmat.cols)
		throw std::exception("Matrix to rotate must be square.");

	cv::Mat retmat;
	retmat = srcmat.clone();

	for (int i = 0; i < srcmat.rows; i++) {
		for (int j = 0; j < srcmat.cols; j++)
			retmat.at<float>(srcmat.rows - 1 - i, srcmat.cols - 1 - j) = srcmat.at<float>(i, j);

		printf("\n");
	}

	return retmat;
}

cv::Mat sim::filterGauss(cv::Mat const operand, int ksize, float sigma, float mu) { 
	if (ksize % 2 != 1 || ksize < 0)
		throw std::exception("Illegal kernel size.");
	cv::Mat gaussmat(ksize, ksize, CV_32F);

	std::vector<float> gaussVec = gaussKernel(ksize, 1, 0);

	cv::Mat xvec(ksize, 1, CV_32F);
	cv::Mat yvec(1, ksize, CV_32F);
	float sumofVec = 0;

	for (int i = 0; i < ksize; i++) {
		xvec.at<float>(i, 0) = yvec.at<float>(0, i) = gaussVec[i];
		sumofVec += gaussVec[i];
	}

	gaussmat = xvec * yvec;

	gaussmat = gaussmat * sumofVec;

	cv::Mat filterOper = sim::convolution2D(operand, gaussmat);
	
	return filterOper;
}

std::vector<float> sim::gaussKernel(float kernel_size, float sigma, float mu) {  //http://dev.theomader.com/gaussian-kernel-calculator/
	auto erf = [](float x) {
		float a1 = 0.254829592;
		float a2 = -0.284496736;
		float a3 = 1.421413741;
		float a4 = -1.453152027;
		float a5 = 1.061405429;
		float p = 0.3275911;

		float sign = 1;
		if (x < 0)
			sign = -1;
		x = abs(x);

		float t = 1.0 / (1.0 + p * x);
		float y = 1.0 - (((((a5 * t + a4) * t) + a3) * t + a2) * t + a1) * t * exp(-x * x);

		return sign * y;
	};

	float sqrt_2 = sqrt(2);

	auto def_int_gaussian = [&erf, &sqrt_2](float x, float mu, float sigma) {		
		return 0.5 * erf((x - mu) / (sqrt_2 * sigma));
	};

	auto start_x = -(kernel_size / 2);
	auto end_x = (kernel_size / 2);
	int step = 1;

	std::vector<float> coeff;
	float last_int = def_int_gaussian(start_x, mu, sigma);

	for (float i = start_x; i < end_x; i += step) {
		float new_int = def_int_gaussian(i + step, mu, sigma);
		coeff.push_back(new_int - last_int);
		last_int = new_int;
	}

	float sum = 0;
	for (float const i : coeff) {
		sum += i;
	}

	for (float &i:coeff) {
		i /= sum;
	}

	return coeff;
}

float sim::sumOfVectorMembers(std::vector<float> operand) {
	float vecoperator = 0;
	for (float iter : operand) {
		vecoperator += iter;
	}
	return vecoperator;
}

float castTouchar(uchar x) {
	float y = (float)x;
	return y;
}

template <typename T>
std::vector<T> operator+(const std::vector<T>& a, const std::vector<T>& b) {
	if (a.size() != b.size()) {
		throw std::exception("Vectors are not equal to each other.");
	}

	std::vector<T> result;
	result.reserve(a.size());

	std::transform(a.begin(), a.end(), b.begin(),
		std::back_inserter(result), std::plus<T>());
	return result;
}