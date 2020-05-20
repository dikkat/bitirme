#pragma once

#include <fstream>
#include <sstream>
#include <algorithm>
#include "Image.h"

namespace img {
	class Image;
	class Histogram;
}

namespace gen {
	extern std::ostringstream tout;
	void imageTesting(img::Image srcimg);
	void printTesting(std::ostringstream &osoperator);
	class CompareTest{
	public: 
		CompareTest(img::Image* lh, img::Image* rh, float sv);
		img::Image* imglh;
		img::Image* imgrh;
		float simval;
	};
	bool compareBySimilarityValue(const CompareTest& a, const CompareTest& b);
}