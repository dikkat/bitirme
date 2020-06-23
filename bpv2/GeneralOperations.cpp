#include "GeneralOperations.h"

std::ostringstream gen::tout;

void gen::printTesting(std::ostringstream &osoperator) {
	std::fstream testfile;
	testfile.open("C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/testbridge.txt", std::ios::out);
	testfile << tout.str();
	testfile.close();
}

void gen::imageTesting(img::Image srcimg, std::string filename) {
	std::string stroper = "C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/" + filename + ".jpg";
	cv::imwrite(stroper, srcimg.getImageMat());
}

gen::CompareTest::CompareTest(img::Image* lh, img::Image* rh, float sv) {
	imglh = lh;
	imgrh = rh;
	simval = sv;
}

bool gen::compareBySimilarityValue(const CompareTest& a, const CompareTest& b) {
	return a.simval > b.simval;
}

