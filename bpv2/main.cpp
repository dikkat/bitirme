#include "image.h"
#include "sim.h"
#include "MainWindow.h"
#include "iop.h"
#include "linker.h"
#include "dbop.h"
#include <QtWidgets/QApplication>
#include <QDebug>
#include <boost/thread.hpp>
#include <chrono>
#include <thread>
#include <algorithm>

/*
	---------ROADMAP FOR THE PROJECT-------------
	MOVE EDGE DETECTION METHODS TO FEAT.H -- DONE
	ADD THE LAST EDGE OPERATOR -DERICHE- FORGOT ITS NAME -- SKIPPED BUT COME BACK
	ADD CORNER DETECTION -- SKIP NO TIME
	ADD KEYPOINT DETECTION -- SKIPPED BUT DO COME BACK
	ASK STACKOVERFLOW QUESTION ABOUT FFT OUTPUT BEING USELESS -- EZ SOLVED
	ADD HASHING -- EZ DONE BUT IS USELESS
	BUILD MACHINE LEARNING -- COME BACK TO IT
	ASK SO ABOUT XXHASH VALUES BEING DIFFERENT -- NVM FIXED
	BUILD THE DATABASE DETAILS WILL FOLLOW -- BUILD IN COOP WITH GUI
	BUILD GUI DETAILS WILL FOLLOW -- HERE ARE THE DETAILS
		VISUALISE THE GUI - BY GRAPHITE PENCIL(OR WHATEVER KURÞUN KALEM IN ENG IS) IF YOU HAVE TO -- DONE I SUPPOSE
		PLACE ALL THE BUTTONS AND TABS -- DONEISH 
		ADD FUNCTIONS TO THOSE OBJECTS
		DONT USE DEFINITE SIZE USE RATIOS
		ASK SOME QUESTIONS ON SO
			WHY IS TABLEVIEW SHOWING A BOX INSTEAD OF STRING
			WHY IS TABLEVIEW HEADER STRING FAULTY
			WHY IS TABLEVIEW LAGGY
		GO BACK TO LINKER.CPP LINE 29 AND INSERT TO ICONIMAGE
	ADD UHMMMMM WHAT WAS IT YES EDGE COMPARISON METHODS AND MAYBE CORNER ASWELL -- SKIP BUT MUST COME BACK YOU HEAR? MUST
	CHANGE RESIZE PARAMETERS AT DBOP::INSERT_IMAGE FUNCTION -- DONE
	LETS NOT FORGET MULTI THREADING
	CHECK IF POINTERS BE CRAZY AT CORNER HARRIS LAPLACE
	MAKE CORNER COLOR AND RADIUS CUSTOMIZABLE
	ADD ZOOMER FUNC TO PAGE 2

	FINISH UNIVERSITY
	GET A 15K DOLLAR PER MONTH JOB
	FINISH LIFE
*/

int main(int argc, char* argv[])
{
	AllocConsole();
	auto ohye = freopen("conin$", "r", stdin);
	ohye = freopen("conout$", "w", stdout);  //DELETE IN FINAL BUILD THIS WHOLE SEGMENT
	ohye = freopen("conout$", "w", stderr);
	/*
	vecf timeVec;
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
	 << sum / timeVec.size() << std::endl;
	*/
	img::Image ima("C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/ukbench/full/ukbench05661.jpg", cv::IMREAD_COLOR);
	dbop::Database db("bitirme.db");
	iop::setDatabaseClass(db);
	lnkr::setDatabaseClass(db);

	using diriter = std::filesystem::directory_iterator;
	using direntry = std::filesystem::directory_entry;
	//-------------------------------------------------------------------
	
	img::Image imb("C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/audi.jpg", cv::IMREAD_COLOR);
	img::Image imc("C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/audiclipped.jpg", cv::IMREAD_COLOR);

	feat::Histogram hist(imb.getImageMat(), HIST_HSV, 255, 255, 255);

	feat::Edge edgea(imb.getImageMat(), EDGE_SOBEL, &feat::Edge::Canny(31,3.6), 400);
	feat::Edge edgeb(imc.getImageMat(), EDGE_SOBEL, &feat::Edge::Canny(31,3.6), 400);

	iop::FeatureVector fva(&imb, nullptr, &edgea);
	iop::FeatureVector fvb(&imc, nullptr, &edgeb);
	iop::WeightVector wv(&vecf{ 0.6, 0.4 });
	iop::Comparison compVal(&fva, &fvb, &wv); //IT SEEMS WE COOL TEST IT ON LOOP WITH IMAGEREADING
	

	auto temp = edgea.getEdgeMat();
	temp = temp * 4;
	auto tempb = edgeb.getEdgeMat();
	tempb = tempb * 4;
	
	
	/*feat::Corner::Harris cdh = feat::Corner::Harris(3,3,2.4,1.9);
	feat::Corner corner(imb.getImageMat(), &cdh, CORNER_HARLAP, 7);
	auto temp = corner.getCornerMarkedMat();
	gen::imageTesting(temp, "test");*/


	//--------------------------------------------------------------------

	QApplication a(argc, argv);
	MainWindow w = MainWindow(db);
	gen::printTesting(gen::tout);
	w.show();
	return a.exec();
}