#include "GeneralOperations.h"

std::ostringstream gen::tout;

void gen::printTesting(std::ostringstream &osoperator) {
	std::fstream testfile;
	testfile.open("C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/testbridge.txt", std::ios::out);
	testfile << tout.str();
	testfile.close();
}

gen::CompareTest::CompareTest(img::Image* lh, img::Image* rh, float sv) {
	imglh = lh;
	imgrh = rh;
	simval = sv;
}

bool gen::compareBySimilarityValue(const CompareTest& a, const CompareTest& b)
{
	return a.simval < b.simval;
}





