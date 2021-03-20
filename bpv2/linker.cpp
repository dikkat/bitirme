#include "linker.h"

void lnkr::setDatabaseClass(dbop::Database dbObj) {
	lnkr_dbPtr = &dbObj;
}

img::Image lnkr::createImage(const string& dir, int flag) {
	img::Image image(dir, flag);
	if(image.getImageMat().data != NULL)
		lnkr_dbPtr->insert_Image(image);
	return image;
}

img::Image lnkr::createImage(const img::Image& image) {
	lnkr_dbPtr->insert_Image(image);
	return image;
}

void lnkr::addToMainTable(img::Image* image) {
	createImage(*image);
}

img::Image lnkr::setSourceImage(const string& dir, int flag) {
	string temp_str = dir;
	std::replace_if(temp_str.begin(), temp_str.end(), [](char c) { return c == '/'; }, '/');

	img::Image image(temp_str, flag);

	lnkr_dbPtr->delete_GENERAL("sourceimage");

	lnkr_dbPtr->insert_SourceImage(image);
	return image;
}

img::Image lnkr::setSourceImage(const img::Image& src) {
	lnkr_dbPtr->delete_GENERAL("sourceimage");

	lnkr_dbPtr->insert_SourceImage(src);
	return src;
}

img::Image lnkr::setDestinationImage(const string& dir, int flag) {
	string temp_str = dir;
	std::replace_if(temp_str.begin(), temp_str.end(), [](char c) { return c == '/'; }, '/');
	
	img::Image image(temp_str, flag);

	lnkr_dbPtr->delete_GENERAL("destimage");

	lnkr_dbPtr->insert_DestinationImage(image);
	return image;
}

img::Image lnkr::setDestinationImage(const img::Image& src) {
	lnkr_dbPtr->delete_GENERAL("destimage");

	lnkr_dbPtr->insert_DestinationImage(src);
	return src;
}

feat::Histogram* lnkr::setHistogram(img::Image* image_ptr, int flag, int fb, int sb, int tb) {
	feat::Histogram* imgHist = new feat::Histogram(image_ptr->getImageMat(), flag, fb, sb, tb);
	return imgHist;
}

feat::Edge::Canny* lnkr::setEdgeCanny(img::Image* image_ptr, float gauss, float sigma, float thigh, float tlow, const cv::Mat& kernelx, const cv::Mat& kernely) {
	feat::Edge::Canny* edgeCanny = new feat::Edge::Canny(gauss, sigma, thigh, tlow, kernelx, kernely);
	return edgeCanny;
}

feat::Edge* lnkr::setEdge(img::Image* image_ptr, int flag, feat::Edge::Canny* edc) {
	feat::Edge* imgEdge = new feat::Edge(image_ptr->getImageMat(), flag, edc);
	return imgEdge;
}

feat::Corner::Harris* lnkr::setCornerHarris(img::Image* image_ptr, float radius, float squareSize, float sigmai, float sigmad,
	float alpha, cv::Mat kernelx, cv::Mat kernely) {
	feat::Corner::Harris* cornerHarris = new feat::Corner::Harris(radius, squareSize, sigmai, sigmad, alpha, kernelx, kernely);
	return cornerHarris;
}

feat::Corner* lnkr::setCorner(img::Image* image_ptr, feat::Corner::Harris cdh, int flag, int numberofScales, float scaleRatio) {
	feat::Corner* imgCorner = new feat::Corner(image_ptr->getImageMat(), &cdh, flag, numberofScales, scaleRatio);
	return imgCorner;
}

void lnkr::setWeightVector(iop::WeightVector* wvec_ptr) {
	lnkr_dbPtr->delete_GENERAL({ "weightvector" });
	lnkr_dbPtr->insert_WeightVector(*wvec_ptr);
}

void lnkr::setSimilarity(iop::Comparison* cmp_ptr) {
	lnkr_dbPtr->insert_Similarity(*cmp_ptr);
}

float lnkr::getSimilarity(img::Image* lhand, img::Image* rhand) {
	string condition = "srchash='" + std::to_string(lhand->getHash()) +
		"' and desthash='" + std::to_string(rhand->getHash()) + "'";
	auto simVec = lnkr_dbPtr->select_GENERAL({ {"similarity"}, {"similarity"}, {condition} });
	if (simVec[0].size() != 0)
		return std::stof(simVec[0][0]);
	else
		return -1;
}

void lnkr::insertDirectoryToDB(diriter dir, int max) {
	auto thrNum = std::thread::hardware_concurrency();

	std::vector<std::thread> threadVec;
	for (int i = 0; i < thrNum; i++) threadVec.push_back(std::thread());

	std::vector<bool> thrCond;
	for (int i = 0; i < thrNum; i++) thrCond.push_back(true);

	int num = -1;
	auto threadCounter = [&num, &thrNum]() {
		if (num == thrNum - 1)
			num = 0;
		else
			num++;
		return num;
	};
	float sum = 0;

	std::mutex m;

	std::function<void(diriter, int, std::vector<std::thread>&, bool)> task;
	task = [&task, &max, &m, &thrCond, &threadVec, &sum](diriter dir, int i, std::vector<std::thread>& threadVec, bool calledRecurr = false)->void {
		if (sum > max) {
			thrCond[i] = true;
			return;
		}
		for (auto& entry : dir) {
			if (sum > max)
				break;
			if (entry.is_directory())
				task(diriter(entry.path().u8string()), i, threadVec, true);
			else if (entry.is_regular_file() && sum < max) {
				auto pathString = entry.path().u8string();
				if (pathString.find(".jpg") != string::npos) {
					m.lock();
					sum++;
					lnkr::createImage(pathString, cv::IMREAD_COLOR);
					m.unlock();
				}
			}
		}
		if (!calledRecurr) {
			thrCond[i] = true;
		}
		return;

	};

	std::vector<img::Image> imgVec;
	for (auto& entry : dir) {
		if (entry.is_directory()) {
			while (true) {
				int i = threadCounter();
				if (thrCond[i] == true) {
					thrCond[i] = false;
					if (threadVec[i].joinable())
						threadVec[i].join();
					threadVec[i] = std::thread{ task, diriter(entry.path().u8string()), i, std::ref(threadVec), false };
					break;
				}
			}
		}
		else if (entry.is_regular_file()) {
			auto pathString = entry.path().u8string();
			if (pathString.find(".jpg") != string::npos) {
				m.lock();
				lnkr::createImage(pathString, cv::IMREAD_COLOR);
				m.unlock();
			}
		}
	}

	for (int i = 0; i < thrCond.size(); i++)
		if (thrCond[i] != true)
			i = 0;

	for (int i = 0; i < thrNum; i++) {
		if (threadVec[i].joinable()) {
			threadVec[i].join();
		}
	}

	return;
}

void lnkr::deleteImage(img::Image* image_ptr) {
	auto hash = std::to_string(image_ptr->getHash());
	/*auto iconHashes = lnkr_dbPtr->select_GENERAL({ {"iconhash"},{"imageicon"},{"imhash=" + hash} })[0];
	for (auto i : iconHashes) {
		lnkr_dbPtr->delete_GENERAL({ "icon" }, "hash='" + i + "'");
		lnkr_dbPtr->delete_GENERAL({ "imageicon" }, "iconhash='" + i + "'");
	}
	lnkr_dbPtr->delete_GENERAL({ "imagecorner" }, "imhash='" + hash + "'");
	lnkr_dbPtr->delete_GENERAL({ "imageedge" }, "imhash='" + hash + "'");
	lnkr_dbPtr->delete_GENERAL({ "imagehistogram" }, "imhash='" + hash + "'");*/
	lnkr_dbPtr->delete_GENERAL({ "similarity" }, "srcHash='" + hash + "' or destHash='" + hash + "'");
	lnkr_dbPtr->delete_GENERAL({ "image" }, "hash='" + hash + "'");
}

void lnkr::deleteFromSimAndWV() {
	lnkr_dbPtr->delete_GENERAL({ "similarity" });
	lnkr_dbPtr->delete_GENERAL({ "weightvector" });
}

std::vector<string> lnkr::getImageDirs() {
	return lnkr_dbPtr->select_GENERAL({ {"dir"}, {"image"}, {} })[0];
}

string lnkr::getImageDir(const string& hash) {
	string condition = "hash='" + hash + "'";
	auto returnedVec = lnkr_dbPtr->select_GENERAL({ {"dir"}, {"image"}, {condition} });
	if (returnedVec[0].size() == 0)
		throw std::exception("That image has no database record which should not be possible, " \
			"props if you get this message, you caught me.");
	else
		return returnedVec[0][0];
}

iop::Comparison lnkr::getRawComparison(const string& hash, bool source) {
	string condition = "";
	if (source)
		condition = "srchash='" + hash + "'";
	else
		condition = "desthash='" + hash + "'";
	auto returned = lnkr_dbPtr->select_GENERAL({ {"diff_gradm", "diff_gradd", "diff_hgray", "diff_hbgrb", "diff_hbgrg", "diff_hbgrr",
		"diff_hhsvh", "diff_hhsvs", "diff_hhsvv", "diff_hashd", "diff_hashp", "similarity"}, {"similarity"}, {condition} });

	if (returned[0].size() == 0)
		throw std::exception("No similarity record for image.");
	else if (returned[0].size() > 1) {
		lnkr_dbPtr->delete_GENERAL("similarity", condition);
		throw std::exception("Multiple similarity records for image. All records removed for that image. "\
			"Do the comparison operation again.");
	}

	iop::Comparison comp;
	auto& compVec = comp.diff_total;
	for (int i = 0; i < returned.size(); i++) {
		*compVec[i] = std::stof(returned[i][0]);
	}

	return comp;
}

iop::WeightVector lnkr::getWeightVector() {
	auto returned = lnkr_dbPtr->select_GENERAL({ {"w_gradm", "w_gradd", "w_hgray", "w_hbgrb", "w_hbgrg", "w_hbgrr",
		"w_hhsvh", "w_hhsvs", "w_hhsvv", "w_hashd", "w_hashp"}, {"weightvector"}, {} });

	if (returned[0].size() == 0)
		throw std::exception("No weight vector defined currently.");
	else if (returned[0].size() > 1) {
		lnkr_dbPtr->delete_GENERAL("weightvector");
		throw std::exception("Multiple weight vector records in program. All records removed in weight vector table. "\
			"Do the comparison operation again.");
	}
	float* temp = nullptr;
	iop::WeightVector wvec(static_cast<float*>(nullptr));
	auto& wvec_total = wvec.wvv_total;
	int k = 0;
	for (int i = 0; i < returned.size(); i++) {
		for (int j = 0; j < 3; j++) {
			try {
				wvec_total.at(i)->at(j) = std::stof(returned[k][0]);
				k++;
			}
			catch (const std::out_of_range& e) {
				continue;
			}
		}
	}

	return wvec;
}

std::vector<img::Image> lnkr::getImageTable() {
	auto dirVec = lnkr_dbPtr->select_GENERAL({ {"dir"},{"image"}, {} });
	if (dirVec.at(0).size() == 0) {
		std::cout << "Image table is empty." << std::endl;
		return {};
	}

	if (dirVec.size() != 1)
		throw std::exception("Exception at table reading. Illegal count of returned columns(must be 1).");

	std::vector<img::Image> returnVec;
	for (auto &i : dirVec.at(0)) {
		returnVec.push_back(img::Image(i, IMG_EMPTY));
	}

	return returnVec;
}

std::vector<iop::Comparison> lnkr::getSimilarityTable() {
	auto selectVec = lnkr_dbPtr->select_Similarity();

	std::vector<iop::Comparison> compVec;

	if (selectVec.at(0).size() == 0) {
		std::cout << "Similarity table is empty." << std::endl;
		return {};
	}

	for (int i = 0; i < selectVec[0].size(); i++) {
		iop::Comparison comp;
		comp.rhand_dir = selectVec[0][i];
		comp.euc_dist = std::stof(selectVec[1][i]);
		compVec.push_back(comp);
	}

	return compVec;
}