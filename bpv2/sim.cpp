#include "sim.h"

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
std::vector<T> sim::matElementsToVector(cv::Mat operand) {
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

template std::vector<float> sim::matElementsToVector<float>(cv::Mat operand);
template std::vector<uchar> sim::matElementsToVector<uchar>(cv::Mat operand);
template std::vector<cf> sim::matElementsToVector<cf>(cv::Mat operand);

template <typename T>
cv::Mat sim::vectorToMatElementsRowMajor(std::vector<T> operand, int mrows, int mcols, int mtype) {
	cv::Mat matoper(mrows, mcols, mtype);
	for (int j = 0; j < matoper.total(); j++) {
		matoper.at<T>(j) = operand[j];
	}
	return matoper;
}

template cv::Mat sim::vectorToMatElementsRowMajor<float>(std::vector<float> operand, int mrows, int mcols, int mtype);
template cv::Mat sim::vectorToMatElementsRowMajor<cf>(std::vector<cf> operand, int mrows, int mcols, int mtype);
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


void sim::Convolution::fftRosetta(std::valarray<cf>& x) { //https://rosettacode.org/wiki/Fast_Fourier_transform#C.2B.2B
	// DFT
	unsigned int N = x.size(), k = N, n;
	double thetaT = 3.14159265358979323846264338328L / N;
	cf phiT = cf(cos(thetaT), -sin(thetaT)), T;
	while (k > 1) {
		n = k;
		k >>= 1;
		phiT = phiT * phiT;
		T = 1.0L;
		for (unsigned int l = 0; l < k; l++) {
			for (unsigned int a = l; a < N; a += n) {
				unsigned int b = a + k;
				cf t = x[a] - x[b];
				x[a] += x[b];
				x[b] = t * T;
			}
			T *= phiT;
		}
	}
	// Decimate
	unsigned int m = (unsigned int)log2(N);
	for (unsigned int a = 0; a < N; a++) {
		unsigned int b = a;
		// Reverse bits
		b = (((b & 0xaaaaaaaa) >> 1) | ((b & 0x55555555) << 1));
		b = (((b & 0xcccccccc) >> 2) | ((b & 0x33333333) << 2));
		b = (((b & 0xf0f0f0f0) >> 4) | ((b & 0x0f0f0f0f) << 4));
		b = (((b & 0xff00ff00) >> 8) | ((b & 0x00ff00ff) << 8));
		b = ((b >> 16) | (b << 16)) >> (32 - m);
		if (b > a) {
			cf t = x[a];
			x[a] = x[b];
			x[b] = t;
		}
	}
}

void sim::Convolution::ifftRosetta(std::valarray<cf>& x) { //https://rosettacode.org/wiki/Fast_Fourier_transform#C.2B.2B
	x = x.apply(std::conj);

	fftRosetta(x);

	x = x.apply(std::conj);

	x /= x.size();
}


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


cv::Mat sim::Convolution::convolution2D(cv::Mat image, cv::Mat kernel) {
	Convolution convOper;
	cv::Mat imMat, kerMat, imopenCV, keropenCV;

	imMat = convOper.convertToComplexMat(image);
	kerMat = convOper.convertToComplexMat(kernel);

	std::vector<cf> imVec, kerVec, resultVec;

	imVec = matElementsToVector<cf>(imMat);
	kerVec = matElementsToVector<cf>(kerMat);

	float power = log2f(imVec.size());
	if (abs(power - (int)power) == 0)
		power++;
	else
		power = ceil(power);

	convOper.zeroPadding(imVec, power);
	convOper.zeroPadding(kerVec, power);
	
	std::valarray<cf> imCArr(imVec.data(), imVec.size());
	std::valarray<cf> kerCArr(kerVec.data(), kerVec.size());

	convOper.fftRosetta(imCArr);
	convOper.fftRosetta(kerCArr);

	imVec.assign(std::begin(imCArr), std::end(imCArr));
	kerVec.assign(std::begin(kerCArr), std::end(kerCArr));

	cv::dft(image, imopenCV);
	cv::dft(kernel, keropenCV);

	convOper.signalMultiplication(image, kernel);
	resultVec = imVec;
	std::valarray<cf> resCArr(resultVec.data(), resultVec.size());

	convOper.ifftRosetta(resCArr);
	resultVec.assign(std::begin(resCArr), std::end(resCArr));

	cv::Mat resultMat;
	resultMat = vectorToMatElementsRowMajor(resultVec, imMat.rows, imMat.cols, imMat.type());

	std::vector<cv::Mat> matVec;
	cv::split(resultMat, matVec);

	double sprgMAX;
	cv::minMaxLoc(matVec[0], 0, &sprgMAX);
	matVec[0] = matVec[0] / sprgMAX * 255;

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

	std::transform(imVec.begin(), imVec.end(), kerVec.begin(), resultVec.begin(), std::multiplies<cf>());

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

cv::Mat sim::convolution2D(cv::Mat const imageMat, cv::Mat const kernel) {
	cv::Mat oper = channelCheck(imageMat);

	cv::Mat resultMat = convolution2DHelix(oper, kernel);

	return resultMat;
}


cv::Mat sim::convolution2DSeparable(cv::Mat const imageMat, cv::Mat const kernel) {
	auto decompose = [](cv::Mat const M) { //https://www.mathworks.com/matlabcentral/fileexchange/28238-kernel-decomposition
		cv::Mat S, U, VT;
		cv::SVDecomp(M, S, U, VT, cv::SVD::FULL_UV);

		std::vector<cv::Mat> matVec;
		cv::Mat M1(M.rows, 1, M.type());
		cv::Mat M2(1, M.cols, M.type());

		for (int i = 0; i < M.rows; i++) {
			M1.at<float>(i) = U.at<float>(i, 0) * sqrt(S.at<float>(0, 0));
			M2.at<float>(i) = VT.at<float>(0, i) * sqrt(S.at<float>(0, 0));
		}

		matVec.push_back(M1);
		matVec.push_back(M2);
		return matVec;
	};
	
	if (kernel.rows != kernel.cols) {
		throw std::exception("Separable convolution failed.");
	}

	std::vector<cv::Mat> matVec = decompose(kernel);

	cv::Mat* xkernel = &matVec[0];
	cv::Mat* ykernel = &matVec[1];

	for(int i = 0; i < xkernel->total(); i++)
		if(!gen::cmpf(xkernel->at<float>(i), ykernel->at<float>(i), 0.0005))
			throw std::exception("Separable convolution failed.");

	cv::Mat matOperY = cv::Mat::zeros(imageMat.rows, imageMat.cols, CV_32FC1);

	cv::Mat imgOper;
	imageMat.convertTo(imgOper, CV_32FC1);

	for (int i = 0; i < matOperY.rows; i++) {
		for (int j = 0; j < matOperY.cols; j++) {
			for (int k = 0; k < ykernel->total(); k++) {
				int index = i - 1 + k;
				if (index < 0 || index >= imgOper.rows)
					continue;
				matOperY.at<float>(i, j) += imgOper.at<float>(index, j) * ykernel->at<float>(k);
			}
		}
	}

	cv::Mat matOperX = cv::Mat::zeros(matOperY.rows, matOperY.cols, matOperY.type());

	for (int i = 0; i < matOperX.rows; i++) {
		for (int j = 0; j < matOperX.cols; j++) {
			for (int k = 0; k < xkernel->total(); k++) { //fazlalýk deðerler daha sonra kýrpýlýr
				int index = j - 1 + k;
				if (index < 0 || index >= imageMat.cols)
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

cv::Mat sim::convolution2DHelix(cv::Mat const imageMat, cv::Mat kernel) { //https://sites.ualberta.ca/~mostafan/Files/Papers/md_convolution_TLE2009.pdf
	cv::Mat imageOper = imageMat.clone();
	
	float RSIZE = imageOper.rows + kernel.rows - 1;
	float CSIZE = imageOper.cols + kernel.cols - 1;

	cv::Mat imOper = cv::Mat::zeros(RSIZE, CSIZE, CV_32FC1);
	for (int i = 0; i < imageOper.rows; i++) {
		for (int j = 0; j < imageOper.cols; j++) {
			if (imageOper.elemSize() == 1)
				imOper.at<float>(i, j) = (float)imageOper.at<uchar>(i, j);
			else if (imageOper.elemSize() == 4)
				imOper.at<float>(i, j) = imageOper.at<float>(i, j);
			else
				throw std::exception("Illegal imageMat element size.");
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
			kerOper.at<float>(i, j) = kernel.at<float>(i, j);
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
	for (int i = 0; i < imVec.size() + kerVec.size() - 1; i++) {
		int kmin, kmax;

		convVec.push_back(0);

		kmax = (i < imVec.size() - 1) ? i : imVec.size() - 1;
		for (float k : nonZeroKerVec) {
			if ((i - k) < 0 || (i - k) >= imVec.size())
				continue;
			if (k > kmax)
				break;
			convVec[i] += imVec[i - k] * kerVec[k];
		}
	}

	cv::Mat convMat(imageMat.rows + kernel.rows - 1, imageMat.cols + kernel.cols - 1, CV_32FC1);

	for (int i = 0; i < convMat.cols; i++) {
		for (int j = 0; j < convMat.rows; j++) {
			convMat.at<float>(j, i) = convVec[i * convMat.rows + j];
		}
	}

	convMat(cv::Range(ceil(kernel.rows/2), convMat.rows - ceil(kernel.rows / 2)), cv::Range(ceil(kernel.cols / 2), convMat.cols - ceil(kernel.cols / 2))).copyTo(convMat);
	return convMat;
}

cv::Mat sim::convolution2DopenCV(cv::Mat const imageMat, cv::Mat kernel) {
	cv::Mat paddedImage, paddedKernel, imgOper, kerOper;

	imgOper = channelCheck(imageMat);

	kerOper = kernel.clone();

	int m = imgOper.rows + kerOper.rows - 1;
	int n = imgOper.cols + kerOper.cols - 1;
	cv::copyMakeBorder(imgOper, paddedImage, 0, m - imgOper.rows, 0, n - imgOper.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));
	cv::copyMakeBorder(kerOper, paddedKernel, 0, m - kerOper.rows, 0, n - kerOper.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));

	cv::Mat planesImage[] = { cv::Mat_<float>(paddedImage), cv::Mat::zeros(paddedImage.size(), CV_32F) };
	cv::Mat cmpImgMat;
	cv::merge(planesImage, 2, cmpImgMat);
	cv::dft(cmpImgMat, cmpImgMat);

	cv::Mat planesKernel[] = { cv::Mat_<float>(paddedKernel), cv::Mat::zeros(paddedKernel.size(), CV_32F) };
	cv::Mat cmpKerMat;
	cv::merge(planesKernel, 2, cmpKerMat);
	cv::dft(cmpKerMat, cmpKerMat);

	cv::Mat convMat;
	cv::mulSpectrums(cmpImgMat, cmpKerMat, convMat, 0);

	cv::Mat planes[2];
	cv::idft(convMat, convMat);
	cv::split(convMat, planes);

	return planes[0];
}

cv::Mat sim::rotateMatrix180(cv::Mat srcmat)
{
	if (srcmat.rows != srcmat.cols)
		throw std::exception("Matrix to be rotated must be square.");

	cv::Mat retmat;
	retmat = srcmat.clone();

	for (int i = 0; i < srcmat.rows; i++) {
		for (int j = 0; j < srcmat.cols; j++)
			retmat.at<float>(srcmat.rows - 1 - i, srcmat.cols - 1 - j) = srcmat.at<float>(i, j);
	}

	return retmat;
}

cv::Mat sim::filterGauss(cv::Mat const operand, int ksize, float sigma, float mu, bool openCV) {
	if (mu != 0) {
		throw std::exception("Mu value must be zero.");
	}
	if (ksize % 2 != 1 || ksize < 0)
		throw std::exception("Illegal kernel size.");

	cv::Mat gaussmat = gaussKernel(ksize, sigma, mu);	

	cv::Mat filterOper;

	if (openCV) {
		filterOper = sim::convolution2DopenCV(operand, gaussmat);
		filterOper(cv::Range(floor(ksize / 2), filterOper.rows - floor(ksize / 2)), cv::Range(floor(ksize / 2), filterOper.cols - floor(ksize / 2))).copyTo(filterOper);
	}
	else
		filterOper = sim::convolution2D(operand, gaussmat);


	return filterOper;
}

cv::Mat sim::gaussKernel(float kernel_size, float sigma, float mu) {  //http://dev.theomader.com/gaussian-kernel-calculator/
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

	cv::Mat gaussMat(kernel_size, kernel_size, CV_32FC1);

	cv::Mat xvec(kernel_size, 1, CV_32F);
	cv::Mat yvec(1, kernel_size, CV_32F);
	float sumofVec = 0;

	for (int i = 0; i < kernel_size; i++) {
		xvec.at<float>(i, 0) = yvec.at<float>(0, i) = coeff[i];
		sumofVec += coeff[i];
	}

	std::cout << xvec << "\n" << yvec << "\n";

	gaussMat = xvec * yvec;

	std::cout << gaussMat << "\n";

	gaussMat = gaussMat * sumofVec;

	std::cout << gaussMat << "\n";

	return gaussMat;
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

cv::Mat sim::channelCheck(cv::Mat const imageMat) {
	cv::Mat oper;
	if (imageMat.channels() == 3)
		cv::cvtColor(imageMat, oper, cv::COLOR_BGR2GRAY);
	else
		oper = imageMat.clone();
	return oper;
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