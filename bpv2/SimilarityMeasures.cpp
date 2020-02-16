#include "SimilarityMeasures.h"

bool sim::vectorEqualityCheck(std::vector<float> lefthand, std::vector<float> righthand) {
	if (lefthand.size() != righthand.size())
		throw std::exception("Two vectors should be same size");
	if (lefthand.size() == 0 || righthand.size() == 0)
		throw std::exception("Vector size can't be zero.");
	return true;
}

float sim::similarityCosine(std::vector<float> lefthand, std::vector<float> righthand) {
	vectorEqualityCheck(lefthand, righthand);

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
	vectorEqualityCheck(lefthand, righthand);

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
	vectorEqualityCheck(lefthand, righthand);
	
	float dstoperator = 0;
	for (int i = 0; i < lefthand.size(); i++) {
		dstoperator += abs(lefthand[i] - righthand[i]);
	}

	return dstoperator;
}

float sim::distanceEuclidean(std::vector<float> lefthand, std::vector<float> righthand) {
	vectorEqualityCheck(lefthand, righthand);

	float dstoperator = 0;
	for (int i = 0; i < lefthand.size(); i++) {
		dstoperator += pow(lefthand[i] - righthand[i], 2);
	}

	return sqrt(dstoperator);
}

float sim::distanceMinkowski(std::vector<float> lefthand, std::vector<float> righthand, int order) {
	vectorEqualityCheck(lefthand, righthand);

	float dstoperator = 0;
	for (int i = 0; i < lefthand.size(); i++) {
		dstoperator += pow(abs(lefthand[i] - righthand[i]), order);
	}
	dstoperator = pow(dstoperator, (float) 1 / order);
	return dstoperator;
}

float sim::histogramIntersection(std::vector<float> lefthand, std::vector<float> righthand) {
	vectorEqualityCheck(lefthand, righthand);

	float numerator = 0, denominator;

	for (int i = 0; i < lefthand.size(); i++) {
		numerator += MIN(lefthand[i], righthand[i]);
	}

	denominator = sumOfVectorMembers(righthand);
	float hintroperator = numerator / denominator;
	return hintroperator;
}

float sim::crossCorrelation(std::vector<float> lefthand, std::vector<float> righthand) {
	vectorEqualityCheck(lefthand, righthand);

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

float sim::sumOfVectorMembers(std::vector<float> operand) {
	float vecoperator = 0;
	for (float iter : operand) {
		vecoperator += iter;
	}
		return vecoperator;
}

std::vector<float> sim::matToVector(cv::Mat operand) {   //https://stackoverflow.com/questions/26681713/convert-mat-to-array-vector-in-opencv
	std::vector<float> array;

	if (operand.isContinuous())
		array.assign((float*)operand.data, (float*)operand.data + operand.total());

	else
		for (int i = 0; i < operand.rows; ++i)
			array.insert(array.end(), operand.ptr<float>(i), operand.ptr<float>(i) + operand.cols);

	return array;
}