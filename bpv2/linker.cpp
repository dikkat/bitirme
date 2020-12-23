#include "linker.h"

void lnkr::setDatabaseClass(dbop::Database dbObj) {
	lnkr_dbPtr = &dbObj;
}

img::Image lnkr::createImage(std::string dir, int flag) {
	img::Image image(dir,flag);
	lnkr_dbPtr->insert_Image(image);
	return image;
}

img::Image lnkr::setSourceImage(std::string dir, int flag) {
	img::Image image(dir, flag);

	lnkr_dbPtr->delete_GENERAL(std::vector<std::string>{"sourceimage"});
	std::string condition = "hash='" + std::to_string(image.getHash()) + "'";
	lnkr_dbPtr->delete_GENERAL(std::vector<std::string>{"image"}, condition);

	lnkr_dbPtr->insert_SourceImage(image);
	return image;
}

img::Image lnkr::setSourceImage(img::Image src) {

	lnkr_dbPtr->delete_GENERAL(std::vector<std::string>{"sourceimage"});
	std::string condition = "hash='" + std::to_string(src.getHash()) + "'";
	lnkr_dbPtr->delete_GENERAL(std::vector<std::string>{"image"}, condition);

	lnkr_dbPtr->insert_SourceImage(src);
	return src;
}

feat::Histogram* lnkr::setHistogram(img::Image* image_ptr, int flag, int fb, int sb, int tb) {
	XXH64_hash_t histHash = feat::Hash::setHash(nullptr, &std::vector<float>{static_cast<float>(flag), static_cast<float>(fb),
		static_cast<float>(sb), static_cast<float>(tb)});

	feat::Histogram* imgHist = lnkr::getImageHist(image_ptr, histHash);
	if(imgHist == nullptr)
		imgHist = new feat::Histogram(image_ptr->getImageMat(), flag, fb, sb, tb);

	lnkr_dbPtr->insert_Histogram(*imgHist);
	lnkr_dbPtr->insert_ImageHistogram(image_ptr->getHash(), imgHist->getHash());

	return imgHist;
}

feat::Edge::Canny* lnkr::setEdgeCanny(img::Image* image_ptr, float gauss, float sigma, float thigh, float tlow, cv::Mat kernelx, cv::Mat kernely) {
	std::vector<float> getVariablesFloat{gauss, thigh, tlow, sigma};
	std::vector<cv::Mat> getVariablesMat{kernelx, kernely};

	XXH64_hash_t cannyHash = feat::Hash::setHash(&getVariablesMat, &getVariablesFloat);
	XXH64_hash_t edgeHash = feat::Hash::setHash(nullptr, &std::vector<float>{static_cast<float>(EDGE_CANNY)});

	edgeHash = feat::Hash::setHash(std::vector<std::string>{std::to_string(edgeHash), std::to_string(cannyHash)});

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
	XXH64_hash_t edgeHash = feat::Hash::setHash(nullptr, &std::vector<float>{static_cast<float>(flag)});
	XXH64_hash_t hash;

	if (edc != nullptr) {
		std::vector<std::string> hashVec;
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
	std::vector<float> getVariablesFloat{ radius, squareSize, sigmai, sigmad, alpha };
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
	XXH64_hash_t hash = feat::Hash::setHash(nullptr, &std::vector<float>{static_cast<float>(flag), 
		static_cast<float>(numberofScales), scaleRatio});
	std::vector<std::string> hashVec;
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

feat::Histogram* lnkr::getImageHist(img::Image* image_ptr, XXH64_hash_t histHash) {
	std::string condition = "imhash='" + std::to_string(image_ptr->getHash()) + "'";
	std::vector<std::string> histHashVec = lnkr_dbPtr->select_Hash(
		"hist", "imagehistogram", condition);

	std::string hist = "";
	for (std::string i : histHashVec)
		if (i == std::to_string(histHash))
			hist = i;

	if (hist == "")
		return nullptr;

	else {
		condition = "hash='" + std::to_string(histHash) + "'";
		std::vector<std::vector<std::string>> resultVec = lnkr_dbPtr->select_GENERAL(std::vector<std::vector<std::string>>{ 
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
	std::string condition = "imhash='" + std::to_string(image_ptr->getHash()) + "'";
	std::vector<std::string> edgeHashVec = lnkr_dbPtr->select_Hash("edge", "imageedge", condition);

	std::string edge = "";
	for (std::string i : edgeHashVec)
		if (i == std::to_string(edgeHash))
			edge = i;

	if (edge == "")
		return nullptr;

	else {
		condition = "hash='" + std::to_string(edgeHash) + "'";
		feat::Edge::Canny canny;
		std::vector<std::vector<std::string>> resultVec = lnkr_dbPtr->select_GENERAL(std::vector<std::vector<std::string>>{
			{"flag", "edcHash"}, { "edge" }, { condition }});
		if (resultVec[0].size() == 0)
			return nullptr;
		else {
			if (resultVec[1].size() != 0 && std::stoi(resultVec[0][0]) == EDGE_CANNY) {
				condition = "hash='" + resultVec[1][0] + "'";
				std::vector<std::vector<std::string>> cannyVec = lnkr_dbPtr->select_GENERAL(std::vector<std::vector<std::string>>{
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
	std::string condition = "imhash='" + std::to_string(image_ptr->getHash()) + "'";
	std::vector<std::string> edgeHashVec = lnkr_dbPtr->select_Hash("edge", "imageedge", condition);

	std::vector<std::string> cannyHashVec;
	std::string edgeHash = "";

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
		std::vector<std::vector<std::string>> cannyVec = lnkr_dbPtr->select_GENERAL(std::vector<std::vector<std::string>>{
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
	std::string condition = "imhash='" + std::to_string(image_ptr->getHash()) + "'";
	std::vector<std::string> cornerHashVec = lnkr_dbPtr->select_Hash("corner", "imagecorner", condition);

	std::string cornerHash_str = "";
	for (std::string i : cornerHashVec)
		if (i == std::to_string(cornerHash))
			cornerHash_str = i;

	if (cornerHash_str == "")
		return nullptr;

	else {
		condition = "hash='" + cornerHash_str + "'";
		feat::Corner::Harris* harris;
		std::vector<std::vector<std::string>> resultVec = lnkr_dbPtr->select_GENERAL(std::vector<std::vector<std::string>>{
			{"cdhHash", "flag", "numberofscales", "scaleratio"}, { "corner" }, { condition }});
		if (resultVec[0].size() == 0)
			return nullptr;
		else {
			feat::Corner* corner;
			condition = "hash='" + resultVec[0][0] + "'";
			std::vector<std::vector<std::string>> harrisVec = lnkr_dbPtr->select_GENERAL(std::vector<std::vector<std::string>>{
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
	std::string condition = "imhash='" + std::to_string(image_ptr->getHash()) + "'";
	std::vector<std::string> cornerHashVec = lnkr_dbPtr->select_Hash("corner", "imagecorner", condition);

	std::vector<std::string> harrisHashVec;
	std::string cornerHash = "";

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
		std::vector<std::vector<std::string>> harrisVec = lnkr_dbPtr->select_GENERAL(std::vector<std::vector<std::string>>{
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

