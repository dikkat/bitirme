#include "gen.h"

std::ostringstream gen::tout;

void gen::printTesting(std::ostringstream &osoperator) {
	std::fstream testfile;
	testfile.open("C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/testbridge.txt", std::ios::out);
	testfile << tout.str();
	testfile.close();
}

bool gen::cmpf(float A, float B, float epsilon) {
	return (fabs(A - B) < epsilon);
}

void gen::imageTesting(cv::Mat imageMat, string filename) {
	time_t now = time(0);
	tm* ltm = localtime(&now);

	boost::filesystem::create_directory("C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/test/" + std::to_string(1900 + ltm->tm_year) + "-"
		+ std::to_string(1 + ltm->tm_mon) + "-" + std::to_string(ltm->tm_mday) + "/");
	string stroper = "C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/test/" + std::to_string(1900 + ltm->tm_year) + "-" 
		+ std::to_string(1 + ltm->tm_mon) + "-" + std::to_string(ltm->tm_mday) + "/"  + std::to_string(ltm->tm_hour) + "-"
		+ std::to_string(ltm->tm_min) + "-" + std::to_string(1 + ltm->tm_sec) + "--" 
		+ filename + ".jpg";	
	cv::imwrite(stroper, imageMat);
}

cv::Mat gen::realNormalize(cv::Mat operand, int bins) {
	cv::Mat newMat = cv::Mat::zeros(operand.rows, operand.cols, CV_32FC1);
	double max;
	cv::minMaxLoc(operand, nullptr, &max);
	for (int j = 0; j < operand.total(); j++) {
		double binSize = max / static_cast<double>(bins);
		for (int k = 0; k < bins; k++) {
			if (operand.at<float>(j) >= binSize * k && operand.at<float>(j) <= binSize * (k + 1)) {
				newMat.at<float>(j) = k + 1;
				break;
			}
		}
	}
	return newMat;
}

//ONLY FOR STORAGE MOVE TO SOMEWHERE ELSE
//void calculateMeanTimes() {
//	auto resize = [](cv::Mat mat, int size) {
//		cv::Mat matOper = mat.clone();
//		int maximumWidth = size;
//		int maximumHeight = size;
//		cv::resize(matOper, matOper, cv::Size(maximumWidth, maximumHeight));
//		return matOper;
//	};
//
//	auto createMatVector = [&resize]() {
//		img::Image image("C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/waterfalls.jpg", cv::IMREAD_COLOR);
//
//		cv::Mat matOper = image.getImageMat().clone();
//		cv::cvtColor(matOper, matOper, cv::COLOR_BGR2GRAY);
//
//		std::vector<cv::Mat> matVec;
//
//		for (int i = 1; i < 15; i++) {
//			matVec.push_back(resize(matOper, i * 100));
//		}
//
//		return matVec;
//	};
//
//	enum conv_flag { CONV_NORMAL, CONV_SPRABLE, CONV_HELIX, CONV_FFT };
//	std::vector<string> convVec{ "NORMAL", "SEPARABLE", "HELIX", "FFT" };
//
//	auto operation = [&resize](cv::Mat image, int flag, cv::Mat kernel) {
//		switch (flag) {
//		case CONV_NORMAL:
//			sim::convolution2DNormal(image, kernel);
//			break;
//		case CONV_SPRABLE:
//			sim::convolution2DSeparable(image, kernel);
//			break;
//		case CONV_HELIX:
//			sim::convolution2DHelix(image, kernel);
//			break;
//		case CONV_FFT:
//			sim::convolution2DopenCV(image, kernel);
//			break;
//		}
//	};
//
//	std::function<bool(vecf, int)>varianceControl;
//	varianceControl = [](vecf vec, int maxCheck)->bool {
//		auto varianceCheck = [](vecf vec) {
//			float sum = 0, max = 0, min = std::numeric_limits<float>::infinity();
//			for (auto a : vec) {
//				sum += a;
//				if (a > max) max = a;
//				if (a < min) min = a;
//			}
//			float avg = sum / vec.size(), range = max - min;
//			float maxVar = pow(range / 2, 2);
//			for (auto a : vec) {
//				if (a > avg + maxVar) return a;
//				if (a < avg - maxVar) return a;
//			}
//			return static_cast<float>(-1);
//		};
//		auto temp = vec;
//		int check = maxCheck;
//		while (check != 0) {
//			float returned = varianceCheck(temp);
//			if (std::roundf(returned) != -1) {
//				temp.erase(std::remove(temp.begin(), temp.end(), returned), temp.end());
//				check--;
//			}
//			else {
//				return true;
//			}
//		}
//		return false;
//	};
//
//	std::vector<cv::Mat> kernelVec{
//		sim::gaussKernel(1, 1.4, 0), sim::gaussKernel(3, 1.4, 0), sim::gaussKernel(5, 1.4, 0), sim::gaussKernel(7, 1.4, 0) };
//
//	for (auto i : kernelVec) {
//		std::cout << i << "\n";
//	}
//
//	std::fstream logFile;
//	logFile.open("C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/logFile.txt", std::ios::out);
//
//	int X = 35;
//	int tryAgain = 3;
//	std::map<std::pair<string, std::pair<int, int>>, float> benchMarkMap;
//	std::vector<cv::Mat> imgVec = createMatVector();
//	for (int i = 0; i < imgVec.size(); i++) {
//		cv::Mat currMat = imgVec[i];
//		vecf benchmarkVec;
//		logFile << currMat.rows << " x " << currMat.rows << " IMAGE" << "\n";
//		for (int j = 0; j < convVec.size(); j++) {
//			logFile << convVec[j] << " METHOD" << "\n";
//			for (int k = 0; k < kernelVec.size(); k++) {
//				cv::Mat currKer = kernelVec[k];
//				logFile << currKer.rows << " x " << currKer.rows << " KERNEL" << "\n";
//				bool errCheck = false;
//				for (int l = 0; l < X; l++) {
//					auto t1 = std::chrono::steady_clock::now();
//					try {
//						operation(currMat, j, currKer);
//					}
//					catch (std::exception e) {
//						errCheck = true;
//						benchmarkVec.push_back(-10000000);
//						continue;
//					}
//					auto t2 = std::chrono::steady_clock::now();
//					float d_micro = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
//					logFile << d_micro << "\n";
//					benchmarkVec.push_back(d_micro);
//				}
//				float sum = 0;
//				for (auto a : benchmarkVec) {
//					sum += a;
//				}
//				if (!varianceControl(benchmarkVec, roundf(static_cast<float>(X) / 9))) {
//					logFile << "There was a variance breach here.\n";
//					if (tryAgain == 0) {
//						continue;
//					}
//					else {
//						tryAgain--;
//						k--;
//						continue;
//					}
//					sum = -10000000;
//				}
//				tryAgain = 3;
//				benchMarkMap[std::make_pair(convVec[j], std::make_pair(currMat.total(), currKer.total()))] = sum / benchmarkVec.size();
//			}
//		}
//	}
//	std::fstream finalLogFile;
//	finalLogFile.open("C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/finalLog.txt", std::ios::out);
//	for (auto i : kernelVec) {
//		std::cout << i << "\n";
//		finalLogFile << i << "\n";
//	}
//
//	for (auto it = benchMarkMap.cbegin(); it != benchMarkMap.cend(); ++it) {
//		std::cout << it->first.first << "\t\t" << it->first.second.first << "\t" << it->first.second.second << "\t" << it->second << "\n";
//		finalLogFile << it->first.first << "\t\t" << it->first.second.first << "\t" << it->first.second.second << "\t" << it->second << "\n";
//	}
//
//	logFile.close();
//	finalLogFile.close();
//}



/*
	img::Image ima("C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/ukbench00140.jpg", cv::IMREAD_COLOR);
	img::Image imc("C:/desired/path/to/Images/static_outdoor_il_giocco_lucca_italy/IMG_9129.jpg", cv::IMREAD_COLOR);
	cv::Mat kernely;
	feat::Corner::Harris cdh;
	vecf xdddddd;
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
		vecf xdg;
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








/*std::vector<string> histVec{ "SIM_COSSIM", "SIM_MANDIST", "SIM_EUCDIST", "SIM_MINKDIST", "SIM_JACSIM", "SIM_HISINTR", "SIM_CROCOR", "SIM_CSQDIST" };
	for (int i = 0; i < 8; i++) {
		std::vector<std::pair<string, std::pair<float, bool>>> comparisonVec;
		for (auto a : scannedVec) {
			if (a.getImageMat().total() == 0)
				continue;
			auto pf = a.getVariablesString()[1];
			auto ps = iop::calculateHistogramSimilarity(ima, a, 60, 60, 60, HIST_GRAY, i);
			comparisonVec.push_back(std::make_pair(pf, ps));
		}
		std::sort(comparisonVec.begin(), comparisonVec.end(),
			[](std::pair<string, std::pair<float, bool>> lh, std::pair<string, std::pair<float, bool>> rh) {
				if (lh.second.second != rh.second.second) throw std::exception("Nonono");
				return lh.second.second ? lh.second.first > rh.second.first : lh.second.first < rh.second.first;
			}
		);
		for (int j = 0; j < 15; j++) {
			auto a = comparisonVec[j];
			auto mat = cv::imread(a.first, cv::IMREAD_COLOR);
			cv::imwrite("C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/results/" + histVec[i] + "_GRAY " + std::to_string(j) + ".jpg", mat);
			std::cout << a.first << "\t" << histVec[i] << "\t" << a.second.first << "\n";
		}
	}*/






/*std::function<std::vector<img::Image>(diriter, unsigned int)> recurSpanner;
	recurSpanner = [](diriter dir, unsigned int maxSize)-> std::vector<img::Image> {
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
		float sum = 0;
		std::function<void(diriter, int, std::vector<std::thread>&, bool)> task;
		task = [&task, &imgVec2D, &thrCond, &threadVec, &sum, &maxSize](diriter dir, int i, std::vector<std::thread>& threadVec, bool calledRecurr = false)->void {
			if (sum > maxSize) {
				thrCond[i] = true;
				return;
			}
			for (auto& entry : dir) {
				if (entry.is_directory())
					task(diriter(entry.path().u8string()), i, threadVec, true);
				else if (entry.is_regular_file() && sum < maxSize) {
					auto pathString = entry.path().u8string();
					if (pathString.find(".jpg") != string::npos) {
						auto imgOper = img::Image::Image(entry.path().u8string(), cv::IMREAD_COLOR);
						size_t sizeInBytes = imgOper.getImageMat().step[0] * imgOper.getImageMat().rows;
						sum += static_cast<float>(sizeInBytes);
						imgVec2D[i].push_back(imgOper);
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
			if (sum > maxSize)
				break;
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

		for (int i = 0; i < thrNum; i++) {
			if (threadVec[i].joinable()) {
				threadVec[i].join();
			}
		}

		for (std::vector<img::Image> &vec : imgVec2D)
			for (img::Image &img : vec) {
				imgVec.push_back(img);
				vec.erase(vec.begin());
			}

		return imgVec;
	};
	img::Image ima("C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/ukbench/full/ukbench09100.jpg", cv::IMREAD_COLOR);
	auto scannedVec = recurSpanner(diriter("C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/iaprtc12/images"), 10);*/



/*std::fstream logFile("C:/Users/ASUS/source/repos/bpv2/bpv2/logFile.txt", std::ios::in);
	std::ofstream finalLog("C:/Users/ASUS/source/repos/bpv2/bpv2/finalLog.txt", std::ios::out);
	if (finalLog.is_open())
		std::cout << "bravo";
	Log log_arr[14];
	string line;
	stringstream line_stream;
	for (int i = 0; i < 14; i++) {
		line_stream.str(string());
		int img_size;
		std::getline(logFile, line);		
		line_stream << line;
		line_stream >> img_size;
		log_arr[i].img_size = img_size;
		finalLog << img_size << " x " << img_size << " IMAGE" << std::endl;
		if (img_size > 1400)
			throw std::exception();
		line_stream.clear();
		for (int j = 0; j < 4; j++) {
			line_stream.str(string());
			string method;
			std::getline(logFile, line);
			line_stream << line;
			line_stream >> method;
			log_arr[i].method[j] = method.substr(0, method.find(" METHOD"));
			finalLog << method << "\n";
			line_stream.clear();
			for (int k = 0; k < 5; k++) {
				line_stream.str(string());
				int ker_size;
				std::getline(logFile, line);
				line_stream << line;
				line_stream >> ker_size;
				log_arr[i].kernel_size[k] = ker_size;
				finalLog << ker_size << " x " << ker_size << " KERNEL" << "\n";
				if (ker_size > 9)
					throw std::exception();
				float sum = 0;
				line_stream.clear();
				for (int l = 0; l < 35; l++) {
					line_stream.str(string());
					float data;
					std::getline(logFile, line);
					line_stream << line;
					line_stream >> data;
					log_arr[i].data[l] = data;
					sum += data;
					line_stream.clear();
				}
				finalLog << sum / 35 << std::endl;
				std::cout << img_size << "x" << img_size << " " << method.substr(0,7) << " " << ker_size << "x" << ker_size << " "
					<< sum / 35 << std::endl;
				
			}
		}
	}
	finalLog.close();*/