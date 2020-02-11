#include "SimilarityMeasures.h"

float sim::cosineSimilarity(std::vector<float> lefthand, std::vector<float> righthand) {
	if (lefthand.size() != righthand.size())
		throw std::exception("Two vectors should be same size");
	if (lefthand.size() == 0 || righthand.size() == 0)
		throw std::exception("Vector size can't be zero.");

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
float sim::sumOfVectorMembers(std::vector<float> operand) {
	float vecoperator = 0;
	for (float iter : operand) {
		vecoperator += iter;
	}
	return vecoperator;
}

std::vector<float> sim::matToVector(cv::Mat operand) {
	std::vector<float> fncoperator;
	for (int i = 0; i < operand.rows; i++)
		for (int j = 0; j < operand.cols; j++)
			fncoperator.push_back(operand.at<float>(i, j));
	return fncoperator;
}