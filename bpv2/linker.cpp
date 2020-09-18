#include "linker.h"

void lnkr::setDatabaseClass(dbop::Database dbObj) {
	lnkr_dbPtr = &dbObj;
}

img::Image lnkr::createImage(std::string dir, int flag) {
	img::Image image(dir,flag);
	lnkr_dbPtr->insert_Image(image);
	return image;
}

void lnkr::setHistogram(img::Image* image_ptr, int flag, int fb, int sb, int tb) {
	feat::Histogram imgHist(image_ptr->getImageMat(), flag, fb, sb, tb);

	lnkr_dbPtr->insert_Histogram(imgHist);
	lnkr_dbPtr->insert_ImageHistogram(image_ptr->getHash(), imgHist.getHash());
}

//feat::Histogram* lnkr::getImageHist(img::Image* image_ptr) {
//	lnkr_dbPtr->select_Hash("dest", "imhist");
//	
//}

void lnkr::setIcon(img::Image* image_ptr) {
	img::Icon imgIcon(image_ptr->getImageMat());

	lnkr_dbPtr->insert_Icon(imgIcon.getIconMat());
	lnkr_dbPtr->insert_ImageIcon(image_ptr->getHash(), imgIcon.getHash());
}