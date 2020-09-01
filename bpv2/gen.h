#pragma once

#include <fstream>
#include <sstream>
#include <algorithm>
#include <functional>
#include "image.h"
#include <boost/filesystem/operations.hpp>

namespace img {
	class Image;
	class Histogram;
}

namespace gen {
	extern std::ostringstream tout;
	void imageTesting(img::Image srcimg, std::string filename);
	void printTesting(std::ostringstream &osoperator);
}

