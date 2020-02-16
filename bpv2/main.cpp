#include "MainWindow.h"
#include "ImageOperations.h"
#include "DatabaseOperations.h"
#include <QtWidgets/QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
	const char* gszFile = "C:/Users/ASUS/source/repos/bpv2/bpv2/Database.db";
	try {
		img::Image ima("C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/ukbench/full/ukbench00020.jpg", cv::IMREAD_COLOR);

		std::vector<img::Image> asd = img::readImageFolder("C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/ukbench/full/", 
			cv::IMREAD_COLOR, false, 25);
		gen::tout << ima.getImageName() << "\t" << "cossim" 
			<< "\t" << "eucdist" 
			<< "\t" << "mandist" 
			<< "\t" << "minkdist" 
			<< "\t" << "jacsim"
			<< "\t" << "histintr"
			<< std::endl;
		/*std::vector<gen::CompareTest> xdc, xde;*/
		std::vector<float> xdg;
		for (int i = 0; i < asd.size(); i++) {
			/*float xd = iop::calculateSimilarity(asd[i], ima, 10, 4, 4, CALC_BGRHIST, SIM_COSSIM);
			gen::CompareTest xdf(&ima, &asd[i], xd);
			xdc.push_back(xdf);
			xd = iop::calculateSimilarity(asd[i], ima, 10, 4, 4, CALC_BGRHIST, SIM_MANDIST);
			xdf = gen::CompareTest(&ima, &asd[i], xd);
			xde.push_back(xdf);*/
			xdg.push_back(iop::calculateSimilarity(asd[i], ima, 10, 4, 4, CALC_BGRHIST, SIM_COSSIM));
			xdg.push_back(iop::calculateSimilarity(asd[i], ima, 10, 4, 4, CALC_BGRHIST, SIM_EUCDIST));
			xdg.push_back(iop::calculateSimilarity(asd[i], ima, 10, 4, 4, CALC_BGRHIST, SIM_MANDIST));
			iop::setMinkowskiOrder(5);
			xdg.push_back(iop::calculateSimilarity(asd[i], ima, 10, 4, 4, CALC_BGRHIST, SIM_MINKDIST));
			xdg.push_back(iop::calculateSimilarity(asd[i], ima, 10, 4, 4, CALC_BGRHIST, SIM_JACSIM));
			xdg.push_back(iop::calculateSimilarity(asd[i], ima, 10, 4, 4, CALC_BGRHIST, SIM_HISINTR));
			xdg.push_back(iop::calculateSimilarity(asd[i], ima, 10, 4, 4, CALC_BGRHIST, SIM_CROCOR));
			int j = 7;
			gen::tout << asd[i].getImageName() 
				<< "\t" << xdg[i*j] 
				<< "\t" << xdg[i * j + 1] 
				<< "\t" << xdg[i * j + 2] 
				<< "\t" << xdg[i * j + 3] 
				<< "\t" << xdg[i * j + 4] 
				<< "\t" << xdg[i * j + 5]
				<< "\t" << xdg[i * j + 6]
				<< std::endl;
		}
		
		/*for (int i = 0; i < xdc.size(); i++) {
			gen::tout << xdc[i].imgrh->getImageName() << "\t" << xdc[i].simval << "\t" << xde[i].simval << "\t" << xdc[i].simval - xde[i].simval << std::endl;
		}*/
	}

	catch (std::exception e) {
		gen::tout << e.what();
	}
	gen::printTesting(gen::tout);
	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}
