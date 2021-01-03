#include "linker.h"

void lnkr::setDatabaseClass(dbop::Database dbObj) {
	lnkr_dbPtr = &dbObj;
}

img::Image lnkr::createImage(string dir, int flag) {
	for (auto &i : dir)
		if (i == '\\')
			i = '/';
	img::Image image(dir,flag);
	lnkr_dbPtr->insert_Image(image);
	return image;
}

void lnkr::addToMainTable(img::Image* image) {
	auto returned = createImage(image->getVariablesString()[1], cv::IMREAD_COLOR);
	setIcon(&returned);
}

img::Image lnkr::setSourceImage(string dir, int flag) {
	for (auto& i : dir)
		if (i == '\\')
			i = '/';
	img::Image image(dir, flag);

	lnkr_dbPtr->delete_GENERAL(std::vector<string>{"sourceimage"});
	string condition = "hash='" + std::to_string(image.getHash()) + "'";

	lnkr_dbPtr->insert_SourceImage(image);
	return image;
}

img::Image lnkr::setSourceImage(img::Image src) {
	lnkr_dbPtr->delete_GENERAL(std::vector<string>{"sourceimage"});
	string condition = "hash='" + std::to_string(src.getHash()) + "'";
	lnkr_dbPtr->delete_GENERAL(std::vector<string>{"image"}, condition);

	lnkr_dbPtr->insert_SourceImage(src);
	return src;
}

img::Image lnkr::setDestinationImage(string dir, int flag) {
	for (auto& i : dir)
		if (i == '\\')
			i = '/';
	img::Image image(dir, flag);

	lnkr_dbPtr->delete_GENERAL(std::vector<string>{"destimage"});
	string condition = "hash='" + std::to_string(image.getHash()) + "'";

	lnkr_dbPtr->insert_DestinationImage(image);
	return image;
}

img::Image lnkr::setDestinationImage(img::Image src) {
	lnkr_dbPtr->delete_GENERAL(std::vector<string>{"destimage"});
	string condition = "hash='" + std::to_string(src.getHash()) + "'";
	lnkr_dbPtr->delete_GENERAL(std::vector<string>{"image"}, condition);

	lnkr_dbPtr->insert_DestinationImage(src);
	return src;
}

feat::Histogram* lnkr::setHistogram(img::Image* image_ptr, int flag, int fb, int sb, int tb) {
	XXH64_hash_t histHash = feat::Hash::setHash(nullptr, &vecf{static_cast<float>(flag), static_cast<float>(fb),
		static_cast<float>(sb), static_cast<float>(tb)});

	feat::Histogram* imgHist = lnkr::getImageHist(image_ptr, histHash);
	if(imgHist == nullptr)
		imgHist = new feat::Histogram(image_ptr->getImageMat(), flag, fb, sb, tb);

	lnkr_dbPtr->insert_Histogram(*imgHist);
	lnkr_dbPtr->insert_ImageHistogram(image_ptr->getHash(), imgHist->getHash());

	return imgHist;
}

feat::Edge::Canny* lnkr::setEdgeCanny(img::Image* image_ptr, float gauss, float sigma, float thigh, float tlow, cv::Mat kernelx, cv::Mat kernely) {
	vecf getVariablesFloat{gauss, thigh, tlow, sigma};
	std::vector<cv::Mat> getVariablesMat{kernelx, kernely};

	XXH64_hash_t cannyHash = feat::Hash::setHash(&getVariablesMat, &getVariablesFloat);
	XXH64_hash_t edgeHash = feat::Hash::setHash(nullptr, &vecf{static_cast<float>(EDGE_CANNY)});

	edgeHash = feat::Hash::setHash(std::vector<string>{std::to_string(edgeHash), std::to_string(cannyHash)});

	feat::Edge* imgEdge = lnkr::getImageEdge(image_ptr, edgeHash);
	feat::Edge::Canny* edgeCanny = lnkr::getEdgeCanny(image_ptr, cannyHash);

	if (edgeCanny == nullptr) {
		edgeCanny = new feat::Edge::Canny(gauss, sigma, thigh, tlow, kernelx, kernely);
		if (imgEdge == nullptr) {
			imgEdge = new feat::Edge(image_ptr->getImageMat(), EDGE_CANNY, edgeCanny);
		}
		else {
			throw std::exception("Canny table doesn't have the tuple that Edge table claims it has.");
		}
	}
	else {
		if (imgEdge == nullptr) {
			throw std::exception("Canny table has tuple that cannot exist without a matching Edge tuple.");
		}
	}

	lnkr_dbPtr->insert_Edge(*imgEdge);
	delete(imgEdge);
	lnkr_dbPtr->insert_EdgeCanny(*edgeCanny);

	return edgeCanny;
}

feat::Edge* lnkr::setEdge(img::Image* image_ptr, int flag, feat::Edge::Canny* edc) {
	XXH64_hash_t edgeHash = feat::Hash::setHash(nullptr, &vecf{static_cast<float>(flag)});
	XXH64_hash_t hash;

	if (edc != nullptr) {
		std::vector<string> hashVec;
		hashVec.push_back(std::to_string(edgeHash));
		hashVec.push_back(std::to_string(edc->getHash()));
		hash = feat::Hash::setHash(hashVec);
	}
	else {
		hash = edgeHash;
	}

	feat::Edge* imgEdge = lnkr::getImageEdge(image_ptr, hash);
	if (imgEdge != nullptr) {
		if (edc != nullptr)
			lnkr_dbPtr->insert_EdgeCanny(*edc);
		lnkr_dbPtr->insert_Edge(*imgEdge);		
		lnkr_dbPtr->insert_ImageEdge(image_ptr->getHash(), imgEdge->getHashVariables()[0]);
	}

	imgEdge = new feat::Edge(image_ptr->getImageMat(), flag, edc);

	return imgEdge;
}

feat::Corner::Harris* lnkr::setCornerHarris(img::Image* image_ptr, float radius, float squareSize, float sigmai, float sigmad, 
	float alpha, cv::Mat kernelx, cv::Mat kernely) {
	vecf getVariablesFloat{ radius, squareSize, sigmai, sigmad, alpha };
	std::vector<cv::Mat> getVariablesMat{ kernelx, kernely };

	XXH64_hash_t harrisHash = feat::Hash::setHash(&getVariablesMat, &getVariablesFloat);

	feat::Corner::Harris* cornerHarris = lnkr::getCornerHarris(image_ptr, harrisHash);

	if (cornerHarris == nullptr) {
		cornerHarris = new feat::Corner::Harris(radius, squareSize, sigmai, sigmad, alpha, kernelx, kernely);
	}

	lnkr_dbPtr->insert_CornerHarris(*cornerHarris);

	return cornerHarris;
}

feat::Corner* lnkr::setCorner(img::Image* image_ptr, feat::Corner::Harris cdh, int flag, int numberofScales, float scaleRatio) {
	XXH64_hash_t hash = feat::Hash::setHash(nullptr, &vecf{static_cast<float>(flag), 
		static_cast<float>(numberofScales), scaleRatio});
	std::vector<string> hashVec;
	hashVec.push_back(std::to_string(hash));
	hashVec.push_back(std::to_string(cdh.getHash()));
	hash = feat::Hash::setHash(hashVec);

	feat::Corner* imgCorner = lnkr::getImageCorner(image_ptr, hash);
	if (imgCorner != nullptr) {
		lnkr_dbPtr->insert_Corner(*imgCorner);
		lnkr_dbPtr->insert_ImageCorner(image_ptr->getHash(), imgCorner->getHashVariables()[0]);
	}

	imgCorner = new feat::Corner(image_ptr->getImageMat(), &cdh, flag, numberofScales, scaleRatio);

	return imgCorner;
}

void lnkr::setIcon(img::Image* image_ptr) {
	img::Icon imgIcon(image_ptr->getImageMat());

	lnkr_dbPtr->insert_Icon(imgIcon.getIconMat());
	lnkr_dbPtr->insert_ImageIcon(image_ptr->getHash(), imgIcon.getHash());
}

void lnkr::setSimilarity(iop::Comparison* cmp_ptr) {
	lnkr_dbPtr->insert_Similarity(*cmp_ptr);	
}

feat::Histogram* lnkr::getImageHist(img::Image* image_ptr, XXH64_hash_t histHash) {
	string condition = "imhash='" + std::to_string(image_ptr->getHash()) + "'";
	std::vector<string> histHashVec = lnkr_dbPtr->select_Hash(
		"hist", "imagehistogram", condition);

	string hist = "";
	for (string i : histHashVec)
		if (i == std::to_string(histHash))
			hist = i;

	if (hist == "")
		return nullptr;

	else {
		condition = "hash='" + std::to_string(histHash) + "'";
		std::vector<std::vector<string>> resultVec = lnkr_dbPtr->select_GENERAL(std::vector<std::vector<string>>{ 
			{"flag", "fbin", "sbin", "tbin"}, { "histogram" }, { condition }});
		if (resultVec[0].size() == 0)
			return nullptr;
		else {
			auto *histPtr = new feat::Histogram(image_ptr->getImageMat(), std::stof(resultVec[0][0]), std::stof(resultVec[1][0]),
				std::stof(resultVec[2][0]), std::stof(resultVec[3][0]));
			return histPtr;
		}
	}
}

//CHECK AGAIN
feat::Edge* lnkr::getImageEdge(img::Image* image_ptr, XXH64_hash_t edgeHash) {
	string condition = "imhash='" + std::to_string(image_ptr->getHash()) + "'";
	std::vector<string> edgeHashVec = lnkr_dbPtr->select_Hash("edge", "imageedge", condition);

	string edge = "";
	for (string i : edgeHashVec)
		if (i == std::to_string(edgeHash))
			edge = i;

	if (edge == "")
		return nullptr;

	else {
		condition = "hash='" + std::to_string(edgeHash) + "'";
		feat::Edge::Canny canny;
		std::vector<std::vector<string>> resultVec = lnkr_dbPtr->select_GENERAL(std::vector<std::vector<string>>{
			{"flag", "edcHash"}, { "edge" }, { condition }});
		if (resultVec[0].size() == 0)
			return nullptr;
		else {
			if (resultVec[1].size() != 0 && std::stoi(resultVec[0][0]) == EDGE_CANNY) {
				condition = "hash='" + resultVec[1][0] + "'";
				std::vector<std::vector<string>> cannyVec = lnkr_dbPtr->select_GENERAL(std::vector<std::vector<string>>{
					{"gausskernelsize", "sigma", "thigh", "tlow", "kernelx", "kernely"}, { "edgecanny" }, { condition }});
				if (cannyVec[0].size() != 0) {
					cv::Mat kernelx = dbop::deserializeMat(cannyVec[4][0]);
					cv::Mat kernely = dbop::deserializeMat(cannyVec[5][0]);
					canny = feat::Edge::Canny(std::stof(cannyVec[0][0]), std::stof(cannyVec[1][0]), std::stof(cannyVec[2][0]),
						std::stof(cannyVec[3][0]), kernelx, kernely);
					auto* edgePtr = new feat::Edge(image_ptr->getImageMat(), std::stoi(resultVec[0][0]), &canny);
					return edgePtr;
				}
				else
					throw std::exception("Edge object has not null canny hash, but that hash isn't on Canny table.");
			}
			else {
				auto* edgePtr = new feat::Edge(image_ptr->getImageMat(), std::stoi(resultVec[0][0]), nullptr);
				return edgePtr;
			}
		}
	}
}

feat::Edge::Canny* lnkr::getEdgeCanny(img::Image* image_ptr, XXH64_hash_t cannyHash) {
	string condition = "imhash='" + std::to_string(image_ptr->getHash()) + "'";
	std::vector<string> edgeHashVec = lnkr_dbPtr->select_Hash("edge", "imageedge", condition);

	std::vector<string> cannyHashVec;
	string edgeHash = "";

	if (edgeHashVec.size() == 0) {
		return nullptr;
	}
	else {
		for (int i = 0; i < edgeHashVec.size(); i++) {
			condition = "hash='" + edgeHashVec[i] + "'";
			cannyHashVec = lnkr_dbPtr->select_Hash("edc", "edge", condition);
			for (int j = 0; j < cannyHashVec.size(); j++)
				if (cannyHashVec[j] == std::to_string(cannyHash))
					edgeHash = edgeHashVec[i];
		}
	}

	if (edgeHash == "")
		return nullptr;

	else {
		feat::Edge::Canny* canny;

		condition = "hash='" + std::to_string(cannyHash) + "'";
		std::vector<std::vector<string>> cannyVec = lnkr_dbPtr->select_GENERAL(std::vector<std::vector<string>>{
			{"gausskernelsize", "sigma", "thigh", "tlow", "kernelx", "kernely"}, { "edgecanny" }, { condition }});

		if (cannyVec[0].size() == 0)
			return nullptr;

		cv::Mat kernelx = dbop::deserializeMat(cannyVec[4][0]);
		cv::Mat kernely = dbop::deserializeMat(cannyVec[5][0]);

		canny = new feat::Edge::Canny(std::stof(cannyVec[0][0]), std::stof(cannyVec[1][0]), std::stof(cannyVec[2][0]),
			std::stof(cannyVec[3][0]), kernelx, kernely);
		if (canny->getHash() != cannyHash)
			throw std::exception("Hash calculation is wrong. Check the hash class.");
		else
			return canny;
	}
}

feat::Corner* lnkr::getImageCorner(img::Image* image_ptr, XXH64_hash_t cornerHash) {
	string condition = "imhash='" + std::to_string(image_ptr->getHash()) + "'";
	std::vector<string> cornerHashVec = lnkr_dbPtr->select_Hash("corner", "imagecorner", condition);

	string cornerHash_str = "";
	for (string i : cornerHashVec)
		if (i == std::to_string(cornerHash))
			cornerHash_str = i;

	if (cornerHash_str == "")
		return nullptr;

	else {
		condition = "hash='" + cornerHash_str + "'";
		feat::Corner::Harris* harris;
		std::vector<std::vector<string>> resultVec = lnkr_dbPtr->select_GENERAL(std::vector<std::vector<string>>{
			{"cdhHash", "flag", "numberofscales", "scaleratio"}, { "corner" }, { condition }});
		if (resultVec[0].size() == 0)
			return nullptr;
		else {
			feat::Corner* corner;
			condition = "hash='" + resultVec[0][0] + "'";
			std::vector<std::vector<string>> harrisVec = lnkr_dbPtr->select_GENERAL(std::vector<std::vector<string>>{
				{"radius", "squaresize", "sigmai", "sigmad", "alpha", "kernelx", "kernely"}, { "cornerharris" }, { condition }});

			if (harrisVec[0].size() == 0) {
				throw std::exception("Harris hash at corner table doesn't have a corresponding record in harris table.");
			}
			else {
				cv::Mat kernelx = dbop::deserializeMat(harrisVec[5][0]);
				cv::Mat kernely = dbop::deserializeMat(harrisVec[6][0]);

				harris = new feat::Corner::Harris(std::stof(harrisVec[0][0]), std::stof(harrisVec[1][0]), std::stof(harrisVec[2][0]),
					std::stof(harrisVec[3][0]), std::stof(harrisVec[4][0]), kernelx, kernely);

				corner = new feat::Corner(image_ptr->getImageMat(), harris, std::stoi(resultVec[1][0]),
					std::stoi(resultVec[2][0]), std::stof(resultVec[3][0]));
				delete(harris);
				return corner;
			}
		}
	}
}

feat::Corner::Harris* lnkr::getCornerHarris(img::Image* image_ptr, XXH64_hash_t harrisHash) {
	string condition = "imhash='" + std::to_string(image_ptr->getHash()) + "'";
	std::vector<string> cornerHashVec = lnkr_dbPtr->select_Hash("corner", "imagecorner", condition);

	std::vector<string> harrisHashVec;
	string cornerHash = "";

	if (cornerHashVec.size() == 0) {
		return nullptr;
	}
	else {
		for (int i = 0; i < cornerHashVec.size(); i++) {
			condition = "hash='" + cornerHashVec[i] + "'";
			harrisHashVec = lnkr_dbPtr->select_Hash("cdh", "corner", condition);
			for (int j = 0; j < harrisHashVec.size(); j++)
				if (harrisHashVec[j] == std::to_string(harrisHash))
					cornerHash = cornerHashVec[i];
		}
	}

	if (cornerHash == "") {
		return nullptr;
	}
	else {
		feat::Corner::Harris* harris;

		condition = "hash='" + std::to_string(harrisHash) + "'";
		std::vector<std::vector<string>> harrisVec = lnkr_dbPtr->select_GENERAL(std::vector<std::vector<string>>{
			{"radius", "squaresize", "sigmai", "sigmad", "alpha", "kernelx", "kernely"}, { "cornerharris" }, { condition }});

		if (harrisVec[0].size() == 0)
			return nullptr;

		cv::Mat kernelx = dbop::deserializeMat(harrisVec[5][0]);
		cv::Mat kernely = dbop::deserializeMat(harrisVec[6][0]);

		harris = new feat::Corner::Harris(std::stof(harrisVec[0][0]), std::stof(harrisVec[1][0]), std::stof(harrisVec[2][0]),
			std::stof(harrisVec[3][0]), std::stof(harrisVec[4][0]), kernelx, kernely);
		if (harris->getHash() != harrisHash)
			throw std::exception("Hash calculation is wrong. Check the hash class.");
		else
			return harris;
	}
}

float lnkr::getSimilarity(img::Image* lhand, img::Image* rhand) {
	string condition = "srchash='" +std::to_string(lhand->getHash()) +
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
	auto iconHashes = lnkr_dbPtr->select_GENERAL({ {"iconhash"},{"imageicon"},{"imhash=" + hash} })[0];
	for (auto i : iconHashes) {
		lnkr_dbPtr->delete_GENERAL({ "icon" }, "hash='" + i + "'");
		lnkr_dbPtr->delete_GENERAL({ "imageicon" }, "iconhash='" + i + "'");
	}
	lnkr_dbPtr->delete_GENERAL({ "imagecorner" }, "imhash='" + hash + "'");
	lnkr_dbPtr->delete_GENERAL({ "imageedge" }, "imhash='" + hash + "'");
	lnkr_dbPtr->delete_GENERAL({ "imagehistogram" }, "imhash='" + hash + "'");
	lnkr_dbPtr->delete_GENERAL({ "similarity" }, "srcHash='" + hash + "' or destHash='" + hash + "'");
	lnkr_dbPtr->delete_GENERAL({ "image" }, "hash='" + hash + "'");
}