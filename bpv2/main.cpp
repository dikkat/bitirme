#include "sim.h"
#include "MainWindow.h"
#include "iop.h"
#include "linker.h"
#include "dbop.h"
#include <QtWidgets/QApplication>
#include <QDebug>
#include <boost/thread.hpp>
#include <chrono>

/*
	---------ROADMAP FOR THE PROJECT-------------
	MOVE EDGE DETECTION METHODS TO FEAT.H -- DONE
	ADD THE LAST EDGE OPERATOR FORGOT ITS NAME -- SKIPPED
	ADD CORNER DETECTION -- SKIP NO TIME
	ADD KEYPOINT DETECTION -- SKIPPED 
	ASK STACKOVERFLOW QUESTION ABOUT FFT OUTPUT BEING USELESS -- EZ SOLVED
	ADD HASHING
	BUILD THE DATABASE DETAILS WILL FOLLOW
	BUILD GUI DETAILS WILL FOLLOW
	FINISH UNIVERSITY
	GET A 15K DOLLAR PER MONTH JOB
	FINISH LIFE
*/


int main(int argc, char *argv[])
{
	AllocConsole();
	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);  //DELETE IN FINAL BUILD THIS WHOLE SEGMENT
	freopen("conout$", "w", stderr);

	const char* gszFile = "C:/Users/ASUS/source/repos/bpv2/bpv2/Database.db";

	/*
	for (int i = 0; i < xde.total(); i++) {
		float f1 = (float)ima.getImageMat().data[i];
		float f2 = (float)xde.data[i];
		std::cout << xdeaf[i+4] << "\t" << ima.getImageHist()->getNormalizedHistogramMat().at<float>(i,i,i)<< "\t" << xde.at<float>(i, i, i)<< "\t" << f1 << "\t" << f2 << " " << std::endl;
	}
	*/

	/*
	std::vector<float> timeVec;
	for (int i = 0; i < 100; i++) {
		auto t1 = std::chrono::steady_clock::now(); //COMPLETE THE CONVOLUTION OPERATION THEN TEST CONVOLUTION2D FUNCTION TIME COMPLEXITY
		cv::Mat xd = ima.getImageMat().clone();
		cv::Mat kernelx = (cv::Mat_<float>(3, 3) << 1, 0, -1, 1, 0, -1, 1, 0, -1);
		sim::convolution2DHelix(xd, kernelx);
		auto t2 = std::chrono::steady_clock::now();
		float d_micro = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
		timeVec.push_back(d_micro);
	}
	float sum = 0;
	for (int i = 0; i < timeVec.size(); i++)
		sum += timeVec[i];	
	std::cout << sum / timeVec.size() << std::endl;

	for (int i = 0; i < 100; i++) {
		auto t1 = std::chrono::steady_clock::now(); //COMPLETE THE CONVOLUTION OPERATION THEN TEST CONVOLUTION2D FUNCTION TIME COMPLEXITY
		cv::Mat xd = ima.getImageMat().clone();
		cv::Mat kernelx = (cv::Mat_<float>(3, 3) << 1, 0, -1, 1, 0, -1, 1, 0, -1);
		sim::convolution2D(xd, kernelx);
		auto t2 = std::chrono::steady_clock::now();
		float d_micro = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
		timeVec[i] = d_micro;
	}
	sum = 0;
	for (int i = 0; i < timeVec.size(); i++)
		sum += timeVec[i];
	std::cout << sum / timeVec.size() << std::endl;
	*/
	img::Image ima("C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/ukbench00140.jpg", cv::IMREAD_COLOR);
	img::Image imc("C:/desired/path/to/Images/static_outdoor_il_giocco_lucca_italy/IMG_9129.jpg", cv::IMREAD_COLOR);
	cv::Mat kernely;
	dbop::initializeDatabase();
	
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
		/*xdg.push_back(iop::calculateHistogramSimilarity(asd[i], ima, 4, 4, 4, HIST_HSV, SIM_COSSIM));
		xdg.push_back(iop::calculateHistogramSimilarity(asd[i], ima, 4, 4, 4, HIST_HSV, SIM_EUCDIST));
		xdg.push_back(iop::calculateHistogramSimilarity(asd[i], ima, 4, 4, 4, HIST_HSV, SIM_MANDIST));
		iop::setMinkowskiOrder(5);
		xdg.push_back(iop::calculateHistogramSimilarity(asd[i], ima, 4, 4, 4, HIST_HSV, SIM_MINKDIST));
		xdg.push_back(iop::calculateHistogramSimilarity(asd[i], ima, 4, 4, 4, HIST_HSV, SIM_CSQDIST));
		xdg.push_back(iop::calculateHistogramSimilarity(asd[i], ima, 4, 4, 4, HIST_HSV, SIM_JACSIM));
		xdg.push_back(iop::calculateHistogramSimilarity(asd[i], ima, 4, 4, 4, HIST_HSV, SIM_HISINTR));
		xdg.push_back(iop::calculateHistogramSimilarity(asd[i], ima, 4, 4, 4, HIST_HSV, SIM_CROCOR));*/
		/*asd[i].setImageHist(4, 4, 4, HIST_HSV);
		ima.setImageHist(4, 4, 4, HIST_HSV);
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
			<< std::endl;*/
	}
	//bune amk	-Enes kardeþ	
	/*for (int i = 0; i < xdc.size(); i++) {
		gen::tout << xdc[i].imgrh->getImageName() << "\t" << xdc[i].simval << "\t" << xde[i].simval << "\t" << xdc[i].simval - xde[i].simval << std::endl;
	}*/
	

	

	gen::printTesting(gen::tout);
	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}