#include "sim.h"
#include "MainWindow.h"
#include "iop.h"
#include "linker.h"
#include "dbop.h"
#include <QtWidgets/QApplication>
#include <QDebug>
#include <boost/thread.hpp>
#include <chrono>
#include "imgui.h"
#include <thread>

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
	 << sum / timeVec.size() << std::endl;
	*/
	dbop::Database db("bitirme.db");
	iop::setDatabaseClass(db);
	lnkr::setDatabaseClass(db);
	/*
	img::Image ima("C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/ukbench00140.jpg", cv::IMREAD_COLOR);
	img::Image imc("C:/desired/path/to/Images/static_outdoor_il_giocco_lucca_italy/IMG_9129.jpg", cv::IMREAD_COLOR);
	cv::Mat kernely;
	feat::Corner::Harris cdh;
	std::vector<float> xdddddd;
	for (auto a : xdddddd)
		a++;
	for(int i = 0; i < 100; i++)
	try {
		feat::Histogram xd(ima.getImageMat(), 122, 115, 123, HIST_BGR);
		db.insert_Histogram(0,122,115,123);
		db.insert_Histogram(xd);
	}
	catch(...) {
		continue;
	}
	//ADD GETVAR FUNCTIONS TO FEAT CLASSES THEN TEST INSERT FUNCTION
	*/

	using diriter = std::filesystem::directory_iterator;
	using direntry = std::filesystem::directory_entry;
	auto thrNum = std::thread::hardware_concurrency();

	std::vector<std::thread> threadVec;
	for (int i = 0; i < thrNum; i++) threadVec.push_back(std::thread());


	std::vector<std::vector<img::Image>> imgVec2D;
	for (int i = 0; i < thrNum; i++) imgVec2D.push_back(std::vector<img::Image>());


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

	std::function<std::vector<img::Image>(diriter)> recurSpanner;
	recurSpanner = [&imgVec2D, &thrCond, &threadCounter, &threadVec](diriter dir)-> std::vector<img::Image> {
		std::function<void(diriter, int, std::vector < std::thread>&, bool)> task;
		task = [&task, &imgVec2D, &thrCond, &threadVec](diriter dir, int i, std::vector<std::thread>& threadVec, bool calledRecurr = false) {
			for (auto& entry : dir) {
				if(entry.is_directory())
					task(diriter(entry.path().u8string()), i, threadVec, true);
				else if (entry.is_regular_file()) {
					auto pathString = entry.path().u8string();
					if (pathString.find(".jpg") != std::string::npos) {
						auto imgOper = img::Image::Image(entry.path().u8string(), cv::IMREAD_COLOR);
						imgVec2D[i].push_back(imgOper);
					}
				}
			}
			if (!calledRecurr) {
				thrCond[i] = true;
			}
			return true;
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
				auto imgOper = img::Image::Image(entry.path().u8string(), cv::IMREAD_COLOR);
				imgVec.push_back(imgOper);
			}
		}
		return imgVec;
	};

	recurSpanner(diriter("C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/iaprtc12/images"));

	/*std::sort(imgVec.begin(), imgVec.end(), [](img::Image lh, img::Image rh) {
		return lh.getImageMat().total() > rh.getImageMat().total();
		});*/

	/*for (int i = 0; i < imgVec.size(); i++) {
		auto tempVec = imgVec[i].getVariablesString();
		auto tempMat = imgVec[i].getImageMat();
		std::cout << tempVec[0] << "\t" << tempVec[1] << "\t" << tempMat.rows << "x" << tempMat.cols << "\n";
	}*/

	//-----------------------------------------------------------//

	/*std::vector<img::Image> asd = img::readImageFolder("C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/ukbench/full/",
		cv::IMREAD_COLOR, false, 25);

	for (img::Image i : asd) {
		try {
			db.insert_Image(i);
		}
		catch (std::exception e) {
			std::cout << e.what() << std::endl;
			continue;
		}
	}*/

	/*
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
		xdg.push_back(iop::calculateHistogramSimilarity(asd[i], ima, 4, 4, 4, HIST_HSV, SIM_COSSIM));
		xdg.push_back(iop::calculateHistogramSimilarity(asd[i], ima, 4, 4, 4, HIST_HSV, SIM_EUCDIST));
		xdg.push_back(iop::calculateHistogramSimilarity(asd[i], ima, 4, 4, 4, HIST_HSV, SIM_MANDIST));
		iop::setMinkowskiOrder(5);
		xdg.push_back(iop::calculateHistogramSimilarity(asd[i], ima, 4, 4, 4, HIST_HSV, SIM_MINKDIST));
		xdg.push_back(iop::calculateHistogramSimilarity(asd[i], ima, 4, 4, 4, HIST_HSV, SIM_CSQDIST));
		xdg.push_back(iop::calculateHistogramSimilarity(asd[i], ima, 4, 4, 4, HIST_HSV, SIM_JACSIM));
		xdg.push_back(iop::calculateHistogramSimilarity(asd[i], ima, 4, 4, 4, HIST_HSV, SIM_HISINTR));
		xdg.push_back(iop::calculateHistogramSimilarity(asd[i], ima, 4, 4, 4, HIST_HSV, SIM_CROCOR));
		asd[i].setImageHist(4, 4, 4, HIST_HSV);
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
			<< std::endl;
	}*/
	//bune amk	-Enes kardeþ	


	/*for (int i = 0; i < xdc.size(); i++) {
		gen::tout << xdc[i].imgrh->getImageName() << "\t" << xdc[i].simval << "\t" << xde[i].simval << "\t" << xdc[i].simval - xde[i].simval << std::endl;
	}*/
	QApplication a(argc, argv);
	MainWindow w = MainWindow(db);
	gen::printTesting(gen::tout);
	w.show();
	return a.exec();
}