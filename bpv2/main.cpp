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
	LETS NOT FORGET MULTI THREADING -- DONE AND DUSTED
	CHECK IF POINTERS BE CRAZY AT CORNER HARRIS LAPLACE -- DONE
	MAKE CORNER COLOR AND RADIUS CUSTOMIZABLE -- NAH
	IF GRAY DISABLE SECOND AND THIRD BIN -- DONE
	IF NOT ENABLED DISABLE ALL WIDGETS -- DONEY
	PUT RAW COMPARISON VALUES INTO DATABASE !!!!!!!!!!!!!!!HIGH PRIORITY --DONE
	MAKE DIR AN ALTERNATE COLUMN TO ICONS !!!!!!!!!! -- LOL SO IMPORTANT

	ADD ZOOMER FUNC TO PAGE 2 --EVENTUALLY
	CHANGE SERIALIZATION TO CV MAT'S
	MAKE IT POSSIBLE THAT CHANGING FEATURE VECTOR ELEMENTS DONT NEED COMPLETE RECALCULATION
	NORMALIZE DIFFERENCE VALUES

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
	lnkr::setDatabaseClass(db);

	//-------------------------------------------------------------------


	img::Image imb("C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/ukbench/full/ukbench00302.jpg", cv::IMREAD_COLOR);
	/*lnkr::setIcon(&imb);
	auto strvec = db.select_GENERAL({ {"dir"}, {"image"}, {} })[0];
	for (int i = 0; i < 200; i++) {
		auto image = img::Image(strvec[i], cv::IMREAD_COLOR);
		lnkr::setIcon(&image);
	}*/
	iop::Comparator cmp;
	{
		auto tedge = feat::Edge(imb.getImageMat(), EDGE_SOBEL, nullptr);
		auto thgray = feat::Histogram(imb.getImageMat(), HIST_GRAY, 10);
		auto thbgr = feat::Histogram(imb.getImageMat(), HIST_BGR, 10, 10, 10);
		auto thhsv = feat::Histogram(imb.getImageMat(), HIST_HSV, 10, 10, 10);
		auto thash = feat::Hash(imb.getImageMat(), std::make_pair(true, true));
		iop::FeatureVector fva(&imb, &tedge, &thgray, &thbgr, &thhsv, &thash);
		iop::FeatureVector* source = &fva;
		iop::WeightVector wv(true);
		//lnkr::insertDirectoryToDB(diriter("C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/ukbench/full"), 2000);
	}
	//	int j = 0;
	//	std::vector<string> strVec{
	//			"C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/ukbench/full/ukbench00288.jpg",
	//			"C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/ukbench/full/ukbench00943.jpg",
	//			"C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/ukbench/full/ukbench00289.jpg",
	//			"C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/ukbench/full/ukbench00291.jpg",
	//			"C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/ukbench/full/ukbench04028.jpg",
	//			"C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/iaprtc12/images/04/4225.jpg",
	//			"C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/ukbench/full/ukbench00463.jpg",
	//			"C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/ukbench/full/ukbench00522.jpg",
	//			"C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/ukbench/full/ukbench01142.jpg",
	//			"C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/ukbench/full/ukbench01532.jpg",
	//			"C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/ukbench/full/ukbench01653.jpg",
	//			"C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/ukbench/full/ukbench01169.jpg",
	//			"C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/ukbench/full/ukbench02951.jpg",
	//			"C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/ukbench/full/ukbench00290.jpg",
	//			"C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/ukbench/full/ukbench03488.jpg",
	//			"C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/ukbench/full/ukbench00339.jpg",
	//			"C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/ukbench/full/ukbench02384.jpg",
	//			"C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/ukbench/full/ukbench00942.jpg",
	//			"C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/iaprtc12/images/05/5016.jpg",
	//			"C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/ukbench/full/ukbench02263.jpg",
	//			"C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/iaprtc12/images/03/3313.jpg" };
	//	std::vector<iop::Comparison> compVec;
	//	for (int i = 0; i < 20; i++) {
	//		cv::Mat loopMat = cv::imread(strVec[i], cv::IMREAD_COLOR);
	//		img::Image img(strVec[i], cv::IMREAD_COLOR);

	//		feat::Edge* edge = nullptr;
	//		if (source->edge != nullptr) {
	//			edge = new feat::Edge(img.getImageMat(), source->edge->getEdgeFlag(), source->edge->getCannyPtr(),
	//				source->edge->getComparisonValues()[0], source->edge->getComparisonValues()[1],
	//				source->edge->getComparisonValues()[2]);
	//		}

	//		feat::Histogram* hist_gray = nullptr;
	//		if (source->hist_gray != nullptr) {
	//			hist_gray = new feat::Histogram(img.getImageMat(), HIST_GRAY, source->hist_gray->getBin()[0]);
	//		}

	//		feat::Histogram* hist_bgr = nullptr;
	//		if (source->hist_bgr != nullptr) {
	//			hist_bgr = new feat::Histogram(img.getImageMat(), HIST_BGR, source->hist_bgr->getBin()[0],
	//				source->hist_bgr->getBin()[1], source->hist_bgr->getBin()[2]);
	//		}

	//		feat::Histogram* hist_hsv = nullptr;
	//		if (source->hist_hsv != nullptr) {
	//			hist_hsv = new feat::Histogram(img.getImageMat(), HIST_HSV, source->hist_hsv->getBin()[0],
	//				source->hist_hsv->getBin()[1], source->hist_hsv->getBin()[2]);
	//		}

	//		feat::Hash* perc_hash = nullptr;
	//		if (source->perc_hash != nullptr) {
	//			perc_hash = new feat::Hash(img.getImageMat(), std::make_pair(
	//				source->perc_hash->getHashVariables().first != NULL ? true : false,
	//				source->perc_hash->getHashVariables().second != NULL ? true : false));
	//		}

	//		iop::FeatureVector rhand(&img, nullptr, edge, hist_gray, hist_bgr, hist_hsv, perc_hash);
	//		iop::Comparison cmptemp(source, &rhand, &wv);
	//		lnkr::setSimilarity(&cmptemp);
	//		lnkr::setIcon(&img);
	//		compVec.push_back(cmptemp);
	//	}
	//}
	//--------------------------------------------------------------------

	QApplication a(argc, argv);
	MainWindow w = MainWindow(db);
	gen::printTesting(gen::tout);
	w.show();
	return a.exec();
}