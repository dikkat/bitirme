#include "gen.h"

std::ostringstream gen::tout;
float counter = 0;

void gen::printTesting(std::ostringstream &osoperator) {
	std::fstream testfile;
	testfile.open("C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/testbridge.txt", std::ios::out);
	testfile << tout.str();
	testfile.close();
}

void gen::imageTesting(img::Image srcimg, std::string filename) {
	time_t now = time(0);
	tm* ltm = localtime(&now);

	std::string stroper = "C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/test/" + std::to_string(1900 + ltm->tm_year) + "-" 
		+ std::to_string(1 + ltm->tm_mon) + "-" + std::to_string(ltm->tm_mday) + "-"  + std::to_string(ltm->tm_hour) + "-"
		+ std::to_string(ltm->tm_min) + "-" + std::to_string(1 + ltm->tm_sec) + "--" 
		+ filename + ".jpg";
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



