#include "SimilarityMeasures.h"

template<typename T>
bool sim::vectorSizeEqualityCheck(std::vector<T> lefthand, std::vector<T> righthand) {
	if (lefthand.size() != righthand.size())
		throw std::exception("Two vectors should be same size");
	if (lefthand.size() == 0 || righthand.size() == 0)
		throw std::exception("Vector size can't be zero.");
	return true;
}

template bool sim::vectorSizeEqualityCheck<float>(std::vector<float> lefthand, std::vector<float> righthand);
template bool sim::vectorSizeEqualityCheck<cf>(std::vector<cf> lefthand, std::vector<cf> righthand);



float sim::similarityCosine(std::vector<float> lefthand, std::vector<float> righthand) {
	vectorSizeEqualityCheck(lefthand, righthand);

	std::vector<float> prdvec, lsqvec, rsqvec;
	float vecoperator = 0;

	for (int i = getI(lefthand); i < lefthand.size(); i++) {
		vecoperator = lefthand[i] * righthand[i];
		prdvec.push_back(vecoperator);

		vecoperator = std::pow(lefthand[i], 2);
		lsqvec.push_back(vecoperator);

		vecoperator = std::pow(righthand[i], 2);
		rsqvec.push_back(vecoperator);
	}

	float numerator = sumOfVectorMembers(prdvec, getI(lefthand));
	float denominator = std::sqrt(sumOfVectorMembers(lsqvec, getI(lefthand))) * std::sqrt(sumOfVectorMembers(rsqvec, getI(lefthand)));
	float cosoperator = numerator / denominator;

	return cosoperator;
}

float sim::similarityJaccard(std::vector<float> lefthand, std::vector<float> righthand) {
	vectorSizeEqualityCheck(lefthand, righthand);

	float numerator = 0;
	float denominator = 0;

	for (int i = getI(lefthand); i < lefthand.size(); i++) {
		numerator += MIN(lefthand[i], righthand[i]);
		denominator += MAX(lefthand[i], righthand[i]);
	}
	
	float jacoperator = numerator / denominator;

	return jacoperator;
}

float sim::distanceManhattan(std::vector<float> lefthand, std::vector<float> righthand) {
	vectorSizeEqualityCheck(lefthand, righthand);
	
	float dstoperator = 0;
	for (int i = getI(lefthand); i < lefthand.size(); i++) {
		dstoperator += abs(lefthand[i] - righthand[i]);
	}

	return dstoperator;
}

float sim::distanceEuclidean(std::vector<float> lefthand, std::vector<float> righthand) {
	vectorSizeEqualityCheck(lefthand, righthand);

	float dstoperator = 0;
	for (int i = getI(lefthand); i < lefthand.size(); i++) {
		dstoperator += pow(lefthand[i] - righthand[i], 2);
	}

	return sqrt(dstoperator);
}

float sim::distanceMinkowski(std::vector<float> lefthand, std::vector<float> righthand, int order) {
	vectorSizeEqualityCheck(lefthand, righthand);

	float dstoperator = 0;
	for (int i = getI(lefthand); i < lefthand.size(); i++) {
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
	for (int i = getI(lefthand); i < lefthand.size(); i++) {
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

	for (int i = getI(lefthand); i < lefthand.size(); i++) {
		numerator += MIN(lefthand[i], righthand[i]);
	}

	denominator = sumOfVectorMembers(righthand, getI(lefthand));
	float hintroperator = numerator / denominator;
	return hintroperator;
}

float sim::crossCorrelation(std::vector<float> lefthand, std::vector<float> righthand) {
	vectorSizeEqualityCheck(lefthand, righthand);

	float numerator = 0, meanlh, meanrh, devoperatorlh, devoperatorrh, crcooperator, denominatorlh = 0, denominatorrh = 0;

	meanlh = sumOfVectorMembers(lefthand, getI(lefthand)) / lefthand.size();
	meanrh = sumOfVectorMembers(righthand, getI(lefthand)) / righthand.size();

	for (int i = getI(lefthand); i < lefthand.size(); i++) {
		devoperatorlh = lefthand[i] - meanlh;
		devoperatorrh = righthand[i] - meanrh;
		numerator += (devoperatorlh) * (devoperatorrh);
		denominatorlh += pow(devoperatorlh, 2);
		denominatorrh += pow(devoperatorrh, 2);
	}

	crcooperator = numerator / sqrt(denominatorlh * denominatorrh);

	return crcooperator;
}

cv::Mat sim::EdgeDetectorCanny::edgeDetectionCanny(cv::Mat const imageMat, sim::EdgeDetectorCanny edcOperator) { //Computer Vision, Mar 2000, Alg 24
	cv::Mat cannyoper;																							 //https://towardsdatascience.com/canny-edge-detection-step-by-step-in-python-computer-vision-b49c3a2d8123
																												 //http://www.tomgibara.com/computer-vision/CannyEdgeDetector.java
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

	gen::imageTesting(img::Image(magMat), "test10");
	cv::Mat nonMaximaMat = edcOperator.suppressNonMaxima(dirMat, magMat);
	gen::imageTesting(img::Image(nonMaximaMat), "test11");
	
	float max = 0;
	for (int i = 0; i < nonMaximaMat.total(); i++)
		if (nonMaximaMat.at<float>(i) > max)
			max = nonMaximaMat.at<float>(i);

	float hThreshold = max * edcOperator.thigh;
	float lThreshold = hThreshold * edcOperator.tlow;

	cv::Mat dtMat(nonMaximaMat.rows, nonMaximaMat.cols, nonMaximaMat.type());
	edcOperator.doubleThreshold(dtMat, nonMaximaMat, max, lThreshold, hThreshold, edcOperator.weakratio);
	gen::imageTesting(img::Image(dtMat), "test12");

	float strong = max;
	float weak = strong * edcOperator.weakratio;

	edcOperator.performHysteresis(dtMat, weak, strong);
	gen::imageTesting(img::Image(dtMat), "test13");
	return dtMat;
}

cv::Mat sim::EdgeDetectorCanny::suppressNonMaxima(cv::Mat &dirMat, cv::Mat &magMat) {
	cv::Mat resultMat = cv::Mat::zeros(magMat.rows, magMat.cols, magMat.type());
	cv::Mat angleMat = cv::Mat::zeros(dirMat.rows, dirMat.cols, dirMat.type());
	for (int i = 0; i < angleMat.total(); i++) {
		angleMat.at<float>(i) = dirMat.at<float>(i) * 180 / M_PI;
		if (angleMat.at<float>(i) < 0)
			angleMat.at<float>(i) += 2 * M_PI;
	}
	for (int i = 0; i < magMat.rows; i++) {
		for (int j = 0; j < magMat.cols; j++) {
			try {
				float q = 255;
				float r = 255;
				float angleOper = angleMat.at<float>(i, j);
				//angle 0
				if (0 <= angleOper < 22.5 || 157.5 <= angleOper <= 180) {
					q = magMat.at<float>(i, j + 1);
					r = magMat.at<float>(i, j - 1);
				}
				//angle 45
				else if (22.5 <= angleMat.at<float>(i, j) < 67.5) {
					q = magMat.at<float>(i + 1, j - 1);
					r = magMat.at<float>(i - 1, j + 1);
				}
				else if (67.5 <= angleMat.at<float>(i, j) < 112.5) {
					q = magMat.at<float>(i + 1, j);
					r = magMat.at<float>(i - 1, j);
				}
				else if (112.5 <= angleMat.at<float>(i, j) < 157.5) {
					q = magMat.at<float>(i - 1, j - 1);
					r = magMat.at<float>(i + 1, j + 1);
				}
				if (magMat.at<float>(i, j) >= q && magMat.at<float>(i, j) >= r)
					resultMat.at<float>(i, j) = magMat.at<float>(i, j);
				else
					resultMat.at<float>(i, j) = 0;
			}
			catch (...) {
				continue;
			}
		}
	}
	return resultMat;
}

void sim::EdgeDetectorCanny::doubleThreshold(cv::Mat& resultMat, cv::Mat const nonMaximaMat, float max, float tlow, float thigh, float weakratio) { //https://towardsdatascience.com/canny-edge-detection-step-by-step-in-python-computer-vision-b49c3a2d8123
	float strong = max;
	float weak = strong * weakratio;
	
	
	for (int i = 0; i < nonMaximaMat.total(); i++) {
		if (nonMaximaMat.at<float>(i) > thigh)
			resultMat.at<float>(i) = strong;
		else if (nonMaximaMat.at<float>(i) < tlow)
			resultMat.at<float>(i) = 0;
		else
			resultMat.at<float>(i) = weak;
	}
}

void sim::EdgeDetectorCanny::performHysteresis(cv::Mat& resultMat, float weak, float strong) {
	for (int i = 0; i < resultMat.rows; i++) {
		for (int j = 0; j < resultMat.cols; j++){
			if (resultMat.at<float>(i, j) == weak) {
				try {
					if ((resultMat.at<float>(i - 1, j - 1) == strong)
						|| (resultMat.at<float>(i - 1, j) == strong)
						|| (resultMat.at<float>(i - 1, j + 1) == strong)
						|| (resultMat.at<float>(i, j - 1) == strong)
						|| (resultMat.at<float>(i, j + 1) == strong)
						|| (resultMat.at<float>(i + 1, j - 1) == strong)
						|| (resultMat.at<float>(i + 1, j) == strong)
						|| (resultMat.at<float>(i + 1, j + 1) == strong))
						resultMat.at<float>(i, j) = strong;
					else
						resultMat.at<float>(i, j) = 0;
				}
				catch (...) {
					continue;
				}
			}
		}
	}
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

	cv::Mat sprgx = convolution2DHelix(imat, kernelx);
	cv::Mat sprgy = convolution2DHelix(imat, kernely);

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
		for (int j = 0; j < sprgDir.cols; j++) {
			sprgDir.at<float>(i, j) = atan2f(sprgy.at<float>(i, j), sprgx.at<float>(i, j));
			if (sprgDir.at<float>(i, j) < 0)
				sprgDir.at<float>(i, j) += M_PI * 2;
		}
	}

	return std::vector<cv::Mat>{sprgMag, sprgDir};
}




template<typename T> 
std::vector<T> sim::matToVector(cv::Mat operand) {   //https://stackoverflow.com/questions/62325615/opencv-data-of-two-mats-are-the-same-but-values-when-retrieved-with-matat-are
	std::vector<T> vecoper;
	uchar* pixelPtr = (uchar*)operand.data;

	vecoper.push_back(operand.dims);
	for (int i = 0; i < operand.dims; i++) {
		vecoper.push_back(operand.size[i]);
	}
	vecoper.push_back(operand.type());

	for (int i = 0; i < operand.total() * operand.elemSize(); i++) {
		vecoper.push_back((T)pixelPtr[i]);
	}

	return vecoper;
}

template std::vector<float> sim::matToVector<float>(cv::Mat operand);
template std::vector<std::complex<float>> sim::matToVector<std::complex<float>>(cv::Mat operand);
template std::vector<uchar> sim::matToVector<uchar>(cv::Mat operand);

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

template cv::Mat sim::vectorToMat<float>(std::vector<float> operand);
template cv::Mat sim::vectorToMat<uchar>(std::vector<uchar> operand);

template <typename T>
std::vector<T> sim::matElemsToVec(cv::Mat operand) {
	std::vector<T> vecOper;
	int cn = operand.channels();
	int lele = operand.total();
	for (int i = 0; i < operand.total(); i++) {
		if (cn == 1)
			vecOper.push_back(operand.at<cv::Vec<T, 1>>(i)[0]);
		else if (cn == 2) {
			if (typeid(T) == typeid(cf)) {
				T xd = operand.at<T>(i);
				vecOper.push_back(xd);
			}
			else
				for (int k = 0; k < cn; k++)
					vecOper.push_back(operand.at<cv::Vec<T, 2>>(i)[k]);
		}
		else if (cn == 3)
			for (int k = 0; k < cn; k++)
				vecOper.push_back(operand.at<cv::Vec<T,3>>(i)[k]);
	}
	return vecOper;
}

template std::vector<float> sim::matElemsToVec<float>(cv::Mat operand);
template std::vector<uchar> sim::matElemsToVec<uchar>(cv::Mat operand);
template std::vector<cf> sim::matElemsToVec<cf>(cv::Mat operand);

template <typename T>
cv::Mat sim::vectorToMatElemsRowMajor(std::vector<T> operand, int mrows, int mcols, int mtype) {
	cv::Mat matoper(mrows, mcols, mtype);

	for (int j = 0; j < matoper.total(); j++) {
		matoper.at<T>(j) = operand[j];
	}
	return matoper;
}

template cv::Mat sim::vectorToMatElemsRowMajor<float>(std::vector<float> operand, int mrows, int mcols, int mtype);
template cv::Mat sim::vectorToMatElemsRowMajor<cf>(std::vector<cf> operand, int mrows, int mcols, int mtype);
//O(log2(N) * N )
/*
cv::Mat sim::fastFourierTransform_2D(cv::Mat const image) { //Introduction to Algorithms, 2009
	auto zeroPadding = [](cv::Mat const operand, unsigned int noofBits) {
		cv::Mat oper = cv::Mat::zeros(sqrt(pow(2, noofBits)), sqrt(pow(2, noofBits)), operand.type());
		for (int i = 0; i < operand.rows; i++) {
			for (int j = 0; j < operand.cols; j++) {
				if (oper.elemSize() == 1) {
					oper.at<uchar>(i, j) = operand.at<uchar>(i, j);
				}
				else if (oper.elemSize() == 4) {
					oper.at<float>(i, j) = operand.at<float>(i, j);
				}
				else
					throw std::exception("Illegal elemsize of image.");
			}
		}
		return oper;
	};

	auto numOfBits = [](cv::Mat const operand) {
		int i = 0;
		while (pow(2, i) < operand.total() || sqrt(pow(2, i)) < operand.rows || sqrt(pow(2, i)) < operand.cols)
			i += 2;

		return i;
	};

	auto rev = [](unsigned int num, unsigned int noofBits)
	{
		unsigned int reverseNum = 0, i, temp;

		for (i = 0; i < noofBits; i++)
		{
			temp = (num & (1 << i));
			if (temp)
				reverseNum |= (1 << ((noofBits - 1) - i));
		}

		return reverseNum;
	};
	
	auto bitReverseCopy = [rev, numOfBits](cv::Mat operand)
	{
		cv::Mat oper(operand.rows, operand.cols, operand.type());
		int j = numOfBits(operand);
		for (int k = 0; k < operand.total(); k++) {
			unsigned int i = rev(k, j);
			if (oper.elemSize() == 1) {
				oper.at<uchar>(k) = operand.at<uchar>(i);
			}
			else if (oper.elemSize() == 4) {
				oper.at<float>(k) = operand.at<float>(i);
			}
		}
		return oper;
	};

	cv::Mat matOper;
	if (image.channels() == 3)
		cv::cvtColor(image, matOper, cv::COLOR_BGR2GRAY);
	else
		matOper = image.clone();

	matOper = bitReverseCopy(zeroPadding(matOper, numOfBits(matOper)));

	matOper.convertTo(matOper, CV_32FC1, 1.0 / 255.0);
	cv::Mat compChannel = cv::Mat::zeros(matOper.rows, matOper.cols, CV_32FC1);

	std::vector<cv::Mat> channels;
	channels.push_back(matOper);
	channels.push_back(compChannel);
	cv::merge(channels, matOper);

	for (int i = 1; i <= log2(matOper.total()); i++) {
		int m = pow(2, i);
		std::complex<float> icomp = (0.0, 1.0);
		std::complex<float> lomeM = std::pow(M_E, (2 * (float)M_PI * icomp / (float)m));
		for (int k = 0; k < matOper.total(); k += m) {
			std::complex<float> lome = 1;
			for (int j = 0; j < m / 2; j++) {
				std::complex<float> t = lome * matOper.at<std::complex<float>>(k + j + m / 2);
				float u = matOper.at<std::complex<float>>(k + j).real();

				matOper.at<std::complex<float>>(k + j) = u + t;
				matOper.at<std::complex<float>>(k + j + m / 2) = u - t;

				lome = lome * lomeM;
			}
		}
	}
	return matOper;
}
*/

/*
void sim::fft(std::vector<cf>& a, bool invert) { //https://cp-algorithms.com/algebra/fft.html
	int n = a.size();
	if (n == 1)
		return;

	std::vector<cf> a0(n / 2), a1(n / 2);
	for (int i = 0; 2 * i < n - 1; i++) {
		a0[i] = a[2 * i];
		a1[i] = a[2 * i + 1]; 
	}
	fft(a0, invert);
	fft(a1, invert);

	double ang = 2 * M_PI / n * (invert ? -1 : 1);
	cf w(1), wn(cos(ang), sin(ang));
	for (int i = 0; 2 * i < n - 1; i++) {
		a[i] = a0[i] + w * a1[i];
		a[i + n / 2] = a0[i] - w * a1[i];
		if (invert) {
			a[i] /= 2;
			a[i + n / 2] /= 2;
		}
		w *= wn;
	}
}
*/
/*
cv::Mat sim::Convolution::convolution2D(cv::Mat image, cv::Mat kernel) {
	Convolution convOper;
	cv::Mat imMat, kerMat; 

	imMat = convertToComplexMat(image);
	kerMat = convertToComplexMat(kernel);

	std::vector<cf> imVec, kerVec, resultVec;

	imVec = matElemsToVec<cf>(imMat);
	kerVec = matElemsToVec<cf>(kerMat);

	float power = log2f(imVec.size());
	if (abs(power - (int)power) == 0)
		power++;
	else
		power = ceil(power);

	convOper.zeroPadding(imVec, power);
	convOper.zeroPadding(kerVec, power);

	convOper.fastFourierTransform2D(imVec, false);
	convOper.fastFourierTransform2D(kerVec, false);

	resultVec = convOper.signalMultiplication(imVec, kerVec);

	convOper.fastFourierTransform2D(resultVec, true);

	cv::Mat resultMat = cv::Mat::zeros(imMat.rows, imMat.cols, imMat.type());
	resultMat = vectorToMatElems(resultVec, imMat.rows, imMat.cols, imMat.type());

	std::vector<cv::Mat> matVec;
	cv::split(resultMat, matVec);

	for (int i = 0; i < resultMat.total(); i++)
		matVec[0].at<float>(i) = roundf(matVec[0].at<float>(i));

	return matVec[0];
}

cv::Mat sim::Convolution::convertToComplexMat(cv::Mat imageMat) {
	cv::Mat matOper;
	if (imageMat.channels() == 3)
		cv::cvtColor(imageMat, matOper, cv::COLOR_BGR2GRAY);
	else
		matOper = imageMat.clone();
	matOper.convertTo(matOper, CV_32FC1);
	cv::Mat compChannel = cv::Mat::zeros(matOper.rows, matOper.cols, CV_32FC1);
	std::vector<cv::Mat> channels;
	channels.push_back(matOper);
	channels.push_back(compChannel);
	cv::merge(channels, matOper);

	return matOper;
}

void sim::Convolution::zeroPadding(std::vector<cf>& a, int power) {
	int p, ioper;

	if (power == -1)
		p = ceil(log2f(a.size()));
	else
		p = power;

	ioper = pow(2, p);
	int size = a.size();
	for (int i = 0; i < ioper - size; i++) {
		a.push_back(0);
	}
}



//FAST COMPLEX MULTIPLICATION = FCM
constexpr cf sim::Convolution::fcm(const cf& lh, const cf& rh) const { //https://www.embedded.com/digital-signal-processing-tricks-fast-multiplication-of-complex-numbers/
	float a = lh.real(), b = lh.imag(), c = rh.real(), d = rh.imag();
	float k1 = a * (c + d), k2 = d * (a + b), k3 = c * (b - a);
	cf result(k1 - k2, k1 + k3);
	return result;
}


std::vector<cf> sim::Convolution::signalMultiplication(std::vector<cf> lh, std::vector<cf> rh) { //IMAGE MUST ALWAYS BE LEFTHAND
	std::vector<cf> imVec = lh, kerVec = rh, resultVec;

	resultVec.resize(imVec.size());

	std::transform(imVec.begin(), imVec.end(), kerVec.begin(), resultVec.begin(), Convolution());

	return resultVec;
}

void sim::Convolution::fastFourierTransform2D(std::vector<cf>& a, bool invert) { //https://cp-algorithms.com/algebra/fft.html
	int n = a.size();

	for (int i = 1, j = 0; i < n; i++) {
		int bit = n >> 1;
		for (; j & bit; bit >>= 1)
			j ^= bit;
		j ^= bit;

		if (i < j)
			swap(a[i], a[j]);
	}

	for (int len = 2; len <= n; len <<= 1) {
		double ang = 2 * M_PI / len * (invert ? -1 : 1);
		cf wlen(cos(ang), sin(ang));
		for (int i = 0; i < n - 1; i += len) {
			cf w(1);
			for (int j = 0; j < len / 2 && i + j < n; j++) {
				cf v = 0;
				cf u = a[i + j];
				if (!(i + j + len / 2 >= n)) {
					v = a[i + j + len / 2] * w;
					a[i + j + len / 2] = u - v;
				}
				a[i + j] = u + v;
				w *= wlen;
			}
		}
	}

	if (invert) {
		for (cf& x : a)
			x /= n;
	}
}
*/

cv::Mat sim::convolution2D(cv::Mat const image, cv::Mat const kernel) {
	cv::Mat oper;
	if (image.channels() == 3)
		cv::cvtColor(image, oper, cv::COLOR_BGR2GRAY);
	else
		oper = image.clone();

	bool separable = false;

	cv::Mat kerOper = rotateMatrix180(kernel);
	if (rankOfMatrix(kerOper) == 1) {
		separable = true;
	}
	cv::Mat resultMat;

	if (separable)
		resultMat = convolution2DSeparable(oper, kerOper);
	else
		resultMat = convolution2DNormal(oper, kerOper);

	return resultMat;
}

cv::Mat sim::convolution2DSeparable(cv::Mat const image, cv::Mat kernel) {
	std::function<int(int, int)> gcd = [&gcd](int a, int b) {
		if (a == 0)
			return b;
		return gcd(b % a, a);
	};

	auto decompose = [&gcd](cv::Mat const M) {
		cv::Mat X = M.row(0).clone();
		cv::Mat Y = M.col(0).clone();
		int nx = X.total();
		int ny = Y.total();
		float gx = X.at<float>(0);
		for (int i = 1; i < nx; i++)
			gx = gcd(gx, X.at<float>(i));
		float gy = Y.at<float>(0);
		for (int i = 1; i < ny; i++)
			gy = gcd(gy, Y.at<float>(i));
		X = X / gx;
		Y = Y / gy;
		float scale = M.at<float>(0, 0) / (X.at<float>(0) * Y.at<float>(0));
		X = X * scale;
		cv::Mat result = Y * X;
		std::cout << Y << std::endl << X << std::endl << result << std::endl << M << std::endl;
		bool valid = std::equal(result.begin<float>(), result.end<float>(), M.begin<float>());
		std::vector<cv::Mat> matVec;
		if (valid) {
			matVec.push_back(X);
			matVec.push_back(Y);
			return matVec;
		}
		else {
			matVec.push_back(M);
			return matVec;
		}
	};
	std::vector<cv::Mat> matVec = decompose(kernel);
	if (matVec.size() == 1) {
		return convolution2DNormal(image, kernel);
	}
	cv::Mat* xkernel = &matVec[0];
	cv::Mat* ykernel = &matVec[1];
	cv::Mat matOperY = cv::Mat::zeros(image.rows, image.cols, CV_32FC1);

	for (int i = 0; i < matOperY.rows; i++) {
		for (int j = 0; j < matOperY.cols; j++) {
			for (int k = 0; k < ykernel->total(); k++) {
				int index = i - 1 + k;
				if (index < 0 || index >= image.rows)
					continue;
				matOperY.at<float>(i, j) += (float)image.at<uchar>(index, j) * ykernel->at<float>(k);
			}
		}
	}

	cv::Mat matOperX = cv::Mat::zeros(matOperY.rows, matOperY.cols, matOperY.type());

	for (int i = 0; i < matOperX.rows; i++) {
		for (int j = 0; j < matOperX.cols; j++) {
			for (int k = 0; k < xkernel->total(); k++) {
				int index = j - 1 + k;
				if (index < 0 || index >= image.cols)
					continue;
				matOperX.at<float>(i, j) += matOperY.at<float>(i, index) * xkernel->at<float>(k);
			}
		}
	}

	return matOperX;
}

cv::Mat sim::convolution2DNormal(cv::Mat const oper, cv::Mat kernel) {  // CPU INTENSIVE FUNCTION -- PARALLELISE
	double kx = kernel.rows, ky = kernel.cols;
	int ix1 = oper.rows, iy1 = oper.cols;
	int ix2 = ix1 + ceil(kx / 2);
	int iy2 = iy1 + ceil(ky / 2);
	cv::Mat convmat = cv::Mat::zeros(ix2, iy2, CV_32FC1);
	for (int i = 0; i < ix2; i++)
		for (int j = 0; j < iy2; j++){
			for (int m = 0; m < kx; m++) {
				for (int n = 0; n < ky; n++) {
					if (i - floor(kx / 2) + m < 0 ||
						j - floor(ky / 2) + n < 0 ||
						i - floor(kx / 2) + m >= ix1 ||
						j - floor(ky / 2) + n >= iy1)
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

						float fop2 = kernel.at<float>(m, n);
						convmat.at<float>(i, j) = convmat.at<float>(i, j) + fop1 * fop2;
					}
				}
			}
			convmat.at<float>(i, j) += 1;
		}
	return convmat;
}

cv::Mat sim::convolution2DHelix(cv::Mat const image, cv::Mat kernel) { //https://sites.ualberta.ca/~mostafan/Files/Papers/md_convolution_TLE2009.pdf
	cv::Mat imageOper;
	cv::cvtColor(image, imageOper, cv::COLOR_BGR2GRAY);
	
	float RSIZE = imageOper.rows + kernel.rows - 1;
	float CSIZE = imageOper.cols + kernel.cols - 1;

	cv::Mat imOper = cv::Mat::zeros(RSIZE, CSIZE, CV_32FC1);
	for (int i = 0; i < imageOper.rows; i++) {
		for (int j = 0; j < imageOper.cols; j++) {
			imOper.at<float>(i, j) = (float)imageOper.at<uchar>(i, j);
		}
	}

	std::vector<float> imVec;
	for (int i = 0; i < CSIZE; i++) {
		for (int j = 0; j < RSIZE; j++) {
			if (j == imageOper.rows && i == imageOper.cols - 1)
				break;
			imVec.push_back(imOper.at<float>(j, i));
		}
	}

	cv::Mat kerOper = cv::Mat::zeros(RSIZE, CSIZE, CV_32FC1);
	for (int i = 0; i < kernel.rows; i++) {
		for (int j = 0; j < kernel.cols; j++) {
			kerOper.at<float>(i, j) = kernel.at<float>(i,j);
		}
	}

	std::vector<float> kerVec;
	std::vector<int> nonZeroKerVec;
	bool check = false;
	float k = 0;
	for (int i = 0; i < CSIZE; i++) {
		if (check)
			break;
		for (int j = 0; j < RSIZE; j++) {
			if (j == kernel.rows && i == kernel.cols - 1) {
				check = true;
				break;
			}
			kerVec.push_back(kerOper.at<float>(j, i));
			if (kerOper.at<float>(j, i) != 0)
				nonZeroKerVec.push_back(k);
			k++;
		}
	}

	std::vector<float> convVec;
	for (int i = 0; i < 12; i++) {
		std::vector<float> Xpp = { -9, -3, -1, 0, 0, 4, -4 };
		std::vector<float> Ypp = { -4, -2, 0, 0, -1, 1 };
		convVec.push_back(0);
		
		std::vector<float>nonZeroKerVec2 = { 0, 1,4,5 };
		int kmin = 0;
		int kmax = (i < Xpp.size() - 1) ? i : Xpp.size() - 1;
		for (int k : nonZeroKerVec2) {
			if ((i - k) < 0 || (i - k) >= Xpp.size())
				continue;
			if (k > kmax)
				break;
			convVec[i] += Xpp[i - k] * Ypp[k];
		}
	}

	convVec.erase(convVec.begin(), convVec.end());
	for (int i = 0; i < imVec.size() + kerVec.size() - 1; i++) {
		int kmin, kmax;

		convVec.push_back(0);

		kmax = (i < imVec.size() - 1) ? i : imVec.size() - 1;
		for (int k : nonZeroKerVec) {
			if ((i - k) < 0 || (i - k) >= imVec.size())
				continue;
			if (k > kmax)
				break;
			convVec[i] += imVec[i - k] * kerVec[k];
		}
	}

	/*cv::Mat ccc = sim::convolution2D(image, kernel);
	gen::imageTesting(img::Image(ccc), "testere2");
	for (int i = 0; i < imageOper.cols; i++) {
		for (int j = 0; j < 10; j++) {
			if (imVec[i * imageOper.rows + j] != ccc.at<float>(j, i));
			std::cout << convVec[i * imageOper.rows + j] << " " << ccc.at<float>(j, i) << std::endl;
		}
		std::cout << std::endl;
	}*/

	cv::Mat convMat(image.rows + kernel.rows - 1, image.cols + kernel.cols - 1, CV_32FC1);

	for (int i = 0; i < convMat.cols; i++) {
		for (int j = 0; j < convMat.rows; j++) {
			convMat.at<float>(j, i) = convVec[i * convMat.rows + j];
		}
	}

	return convMat;
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
	}

	return retmat;
}

cv::Mat sim::filterGauss(cv::Mat const operand, int ksize, float sigma, float mu) { 
	if (ksize % 2 != 1 || ksize < 0)
		throw std::exception("Illegal kernel size.");
	cv::Mat gaussmat(ksize, ksize, CV_32FC1);

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

float sim::sumOfVectorMembers(std::vector<float> operand, int offset) {
	float vecoperator = 0;
	for (int i = offset; i < operand.size(); i++) {
		vecoperator += operand[i];
	}
	return vecoperator;
}



int sim::rankOfMatrix(cv::Mat const mat) { //https://www.geeksforgeeks.org/program-for-rank-of-matrix/
	auto swap = [](cv::Mat mat, int row1, int row2, int col) {
		for (int i = 0; i < col; i++) {
			int temp = mat.at<float>(row1, i);
			mat.at<float>(row1, i) = mat.at<float>(row2, i);
			mat.at<float>(row2, i) = temp;
		}
	};

	cv::Mat matOper = mat.clone();
	int R = matOper.rows;
	int C = matOper.cols;

	int rank = C;

	for (int row = 0; row < rank; row++) {
		if (matOper.at<float>(row, row)) {
			for (int col = 0; col < R; col++) {
				if (col != row) {
					float mult = matOper.at<float>(col, row) / matOper.at<float>(row, row);
					for (int i = 0; i < rank; i++)
						matOper.at<float>(col, i) -= mult * matOper.at<float>(row, i);
				}
			}
		}

		else {
			bool reduce = true;

			for (int i = row + 1; i < R; i++) {
				if (matOper.at<float>(i, row)) {
					swap(matOper, row, i, rank);
					reduce = false;
					break;
				}
			}

			if (reduce) {
				rank--;

				for (int i = 0; i < R; i++)
					matOper.at<float>(i, row) = matOper.at<float>(i, rank);
			}

			row--;
		}
	}
	return rank;
}

template <typename T>
int sim::getI(std::vector<T> operand) {
	return operand[0] + 2 - 1; //num of dims + dim + type - offset
}

template int sim::getI<float>(std::vector<float> operand);
template int sim::getI<uchar>(std::vector<uchar> operand);

/*float castTouchar(uchar x) {
	float y = (float)x;
	return y;
}*/

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