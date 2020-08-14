#include "linker.h"

std::vector<lnkr::ImageFeatures> lnkr::ImgFeatVector;

lnkr::ImageFeatures::ImageFeatures(img::Image* srimg) {
    srcimg = srimg;
}

void lnkr::setImageHist(img::Image* img, int fb, int sb, int tb, int flag) {
    int loc = queryImgFeatVector(img);
    if (loc == -1) {
        ImageFeatures imfeatOper(img);
        imfeatOper.setImageHist= new feat::Histogram(img, fb, sb, tb, flag);
        ImgFeatVector.push_back(imfeatOper);
    }
    else {
        ImageFeatures* imfeatOper = &ImgFeatVector[loc];
        imfeatOper->setImageHist = new feat::Histogram(img, fb, sb, tb, flag);
    }    
}

void lnkr::ImageFeatures::setImageHist(int fb, int sb, int tb, int flag) {
    this->imagehist = new feat::Histogram(this->srcimg, fb, sb, tb, flag);
}

feat::Histogram* lnkr::getImageHist(img::Image* image) {
    int loc = queryImgFeatVector(image);
    ImageFeatures* imfeatOper = &ImgFeatVector[loc];
    return imfeatOper->getImageHist();
}

feat::Histogram* lnkr::ImageFeatures::getImageHist() {
    return this->imagehist;
}

void img::Image::destroyImageHist() {
    this->imagehist == NULL;
}

img::Image* lnkr::ImageFeatures::getSrcImg() {
    return this->srcimg;
}

int lnkr::queryImgFeatVector(img::Image* image) {
    for (int i = 0; i < lnkr::ImgFeatVector.size(); i++)
        if (lnkr::ImgFeatVector[i].getSrcImg() == image)
            return i;
        else
            return -1;
}