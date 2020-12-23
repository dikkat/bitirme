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

	using diriter = std::filesystem::directory_iterator;
	using direntry = std::filesystem::directory_entry;

	auto resize = [](cv::Mat mat, int size) {
		cv::Mat matOper = mat.clone();
		int maximumWidth = size;
		int maximumHeight = size;
		cv::resize(matOper, matOper, cv::Size(maximumWidth, maximumHeight));
		return matOper;
	};

	auto createMatVector = [&resize]() {
		img::Image image("C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/waterfalls.jpg", cv::IMREAD_COLOR);

		cv::Mat matOper = image.getImageMat().clone();
		cv::cvtColor(matOper, matOper, cv::COLOR_BGR2GRAY);

		std::vector<cv::Mat> matVec;

		for (int i = 1; i < 15; i++) {
			matVec.push_back(resize(matOper, i * 100));
		}

		return matVec;
	};

	enum conv_flag { CONV_NORMAL, CONV_SPRABLE, CONV_HELIX, CONV_FFT };
	std::vector<std::string> convVec{ "NORMAL", "SEPARABLE", "HELIX", "FFT" };

	auto operation = [&resize](cv::Mat image, int flag, cv::Mat kernel) {
		switch (flag) {
		case CONV_NORMAL:
			sim::convolution2DNormal(image, kernel);
			break;
		case CONV_SPRABLE:
			sim::convolution2DSeparable(image, kernel);
			break;
		case CONV_HELIX:
			sim::convolution2DHelix(image, kernel);
			break;
		case CONV_FFT:
			sim::convolution2DopenCV(image, kernel);
			break;
		}
	};

	std::function<bool(std::vector<float>, int)>varianceControl;
	varianceControl = [](std::vector<float> vec, int maxCheck)->bool {
		auto varianceCheck = [](std::vector<float> vec) {
			float sum = 0, max = 0, min = std::numeric_limits<float>::infinity();
			for (auto a : vec) {
				sum += a;
				if (a > max) max = a;
				if (a < min) min = a;
			}
			float avg = sum / vec.size(), range = max - min;
			float maxVar = pow(range / 2, 2);
			for (auto a : vec) {
				if (a > avg + maxVar) return a;
				if (a < avg - maxVar) return a;
			}
			return static_cast<float>(-1);
		};
		auto temp = vec;
		int check = maxCheck;
		while (check != 0) {
			float returned = varianceCheck(temp);
			if (std::roundf(returned) != -1) {
				temp.erase(std::remove(temp.begin(), temp.end(), returned), temp.end());
				check--;
			}
			else {
				return true;
			}
		}
		return false;
	};

	std::vector<cv::Mat> kernelVec{
		sim::gaussKernel(1, 1.4, 0), sim::gaussKernel(3, 1.4, 0), sim::gaussKernel(5, 1.4, 0), sim::gaussKernel(7, 1.4, 0) };

	for (auto i : kernelVec) {
		std::cout << i << "\n";
	}

	std::fstream logFile;
	logFile.open("C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/logFile.txt", std::ios::out);

	int X = 35;
	int tryAgain = 3;
	std::map<std::pair<std::string, std::pair<int, int>>, float> benchMarkMap;
	std::vector<cv::Mat> imgVec = createMatVector();
	for (int i = 0; i < imgVec.size(); i++) {
		cv::Mat currMat = imgVec[i];
		std::vector<float> benchmarkVec;
		logFile << currMat.rows << " x " << currMat.rows << " IMAGE" << "\n";
		for (int j = 0; j < convVec.size(); j++) {
			logFile << convVec[j] << " METHOD" << "\n";
			for (int k = 0; k < kernelVec.size(); k++) {
				cv::Mat currKer = kernelVec[k];
				logFile << currKer.rows << " x " << currKer.rows << " KERNEL" << "\n";
				bool errCheck = false;
				for (int l = 0; l < X; l++) {
					auto t1 = std::chrono::steady_clock::now();
					try {
						operation(currMat, j, currKer);
					}
					catch (std::exception e) {
						errCheck = true;
						benchmarkVec.push_back(-10000000);
						continue;
					}
					auto t2 = std::chrono::steady_clock::now();
					float d_micro = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
					logFile << d_micro << "\n";
					benchmarkVec.push_back(d_micro);
				}
				float sum = 0;
				for (auto a : benchmarkVec) {
					sum += a;
				}
				if (!varianceControl(benchmarkVec, roundf(static_cast<float>(X) / 9))) {
					logFile << "There was a variance breach here.\n";
					if (tryAgain == 0) {
						continue;
					}
					else {
						tryAgain--;
						k--;
						continue;
					}
					sum = -10000000;
				}
				tryAgain = 3;
				benchMarkMap[std::make_pair(convVec[j], std::make_pair(currMat.total(), currKer.total()))] = sum / benchmarkVec.size();
			}
		}
	}
	std::fstream finalLogFile;
	finalLogFile.open("C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/finalLog.txt", std::ios::out);
	for (auto i : kernelVec) {
		std::cout << i << "\n";
		finalLogFile << i << "\n";
	}

	for (auto it = benchMarkMap.cbegin(); it != benchMarkMap.cend(); ++it) {
		std::cout << it->first.first << "\t\t" << it->first.second.first << "\t" << it->first.second.second << "\t" << it->second << "\n";
		finalLogFile << it->first.first << "\t\t" << it->first.second.first << "\t" << it->first.second.second << "\t" << it->second << "\n";
	}

	logFile.close();
	finalLogFile.close();


	//--------------------------------------------------------------------

	QApplication a(argc, argv);
	MainWindow w = MainWindow(db);
	gen::printTesting(gen::tout);
	w.show();
	return a.exec();
}