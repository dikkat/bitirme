#include "MainWindow.h"
#include "ImageOperations.h"
#include "DatabaseOperations.h"
#include <QtWidgets/QApplication>
#include <QDebug>
#include <boost/thread.hpp>
#include <chrono>

int main(int argc, char *argv[])
{
	AllocConsole();
	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);  //DELETE IN FINAL BUILD THIS WHOLE SEGMENT
	freopen("conout$", "w", stderr);

	const char* gszFile = "C:/Users/ASUS/source/repos/bpv2/bpv2/Database.db";
	
	img::Image ima("C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/bikesgray.jpg", cv::IMREAD_UNCHANGED);

	/*for (int i = 0; i < xde.total(); i++) {
		
			float f1 = (float)ima.getImageMat().data[i];
			float f2 = (float)xde.data[i];
			std::cout << xdeaf[i+4] << "\t" << ima.getImageHist()->getNormalizedHistogramMat().at<float>(i,i,i)<< "\t" << xde.at<float>(i, i, i)<< "\t" << f1 << "\t" << f2 << " " << std::endl;
		
	}*/
	cv::Mat kernely = sim::edgeDetectionCanny(ima.getImageMat());
	gen::imageTesting(img::Image(kernely), "test");
	
	std::vector<img::Image> asd = img::readImageFolder("C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/ukbench/full/", 
		cv::IMREAD_COLOR, false, 25);
	img::Image imb("C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/ukbench/full/ukbench00088.jpg", cv::IMREAD_COLOR);
	asd.push_back(imb);
	gen::tout << ima.getImageName() << "\t" << "cossim" 
		<< "\t" << "eucdist" 
		<< "\t" << "mandist" 
		<< "\t" << "minkdist"
		<< "\t" << "csqdist"
		<< "\t" << "jacsim"
		<< "\t" << "histintr"
		<< "\t" << "crocor"
		<< "\t" << "CVcsqdist"
		<< std::endl;
	std::vector<float> xdg;
	for (int i = 0; i < asd.size(); i++) {
		xdg.push_back(iop::calculateHistogramSimilarity(asd[i], ima, 4, 4, 4, CALC_HSVHIST, SIM_COSSIM));
		xdg.push_back(iop::calculateHistogramSimilarity(asd[i], ima, 4, 4, 4, CALC_HSVHIST, SIM_EUCDIST));
		xdg.push_back(iop::calculateHistogramSimilarity(asd[i], ima, 4, 4, 4, CALC_HSVHIST, SIM_MANDIST));
		iop::setMinkowskiOrder(5);
		xdg.push_back(iop::calculateHistogramSimilarity(asd[i], ima, 4, 4, 4, CALC_HSVHIST, SIM_MINKDIST));
		xdg.push_back(iop::calculateHistogramSimilarity(asd[i], ima, 4, 4, 4, CALC_HSVHIST, SIM_CSQDIST));
		xdg.push_back(iop::calculateHistogramSimilarity(asd[i], ima, 4, 4, 4, CALC_HSVHIST, SIM_JACSIM));
		xdg.push_back(iop::calculateHistogramSimilarity(asd[i], ima, 4, 4, 4, CALC_HSVHIST, SIM_HISINTR));
		xdg.push_back(iop::calculateHistogramSimilarity(asd[i], ima, 4, 4, 4, CALC_HSVHIST, SIM_CROCOR));
		asd[i].setImageHist(4, 4, 4, CALC_HSVHIST);
		ima.setImageHist(4, 4, 4, CALC_HSVHIST);
		xdg.push_back(cv::compareHist(ima.getImageHist()->getNormalizedHistogramMat(), asd[i].getImageHist()->getNormalizedHistogramMat(), cv::HISTCMP_CHISQR));
		asd[i].destroyImageHist();
		int j = 9;
		gen::tout << asd[i].getImageName() 
			<< "\t" << xdg[i * j]
			<< "\t" << xdg[i * j + 1]
			<< "\t" << xdg[i * j + 2]
			<< "\t" << xdg[i * j + 3]
			<< "\t" << xdg[i * j + 4]
			<< "\t" << xdg[i * j + 5]
			<< "\t" << xdg[i * j + 6]
			<< "\t" << xdg[i * j + 7]
			<< "\t" << xdg[i * j + 8]
			<< std::endl;
	}
		
	/*for (int i = 0; i < xdc.size(); i++) {
		gen::tout << xdc[i].imgrh->getImageName() << "\t" << xdc[i].simval << "\t" << xde[i].simval << "\t" << xdc[i].simval - xde[i].simval << std::endl;
	}*/
	

	

	gen::printTesting(gen::tout);
	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}