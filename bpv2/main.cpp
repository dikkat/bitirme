#include "MainWindow.h"
#include "ImageOperations.h"
#include "DatabaseOperations.h"
#include <QtWidgets/QApplication>
#include <QDebug>


int main(int argc, char *argv[])
{
	bool we = dbop::connectToDatabase("localhost", "vsdev", "1235", "bitirme");
	gen::tout << we << std::endl;
	try {
		img::Image ima("C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/ukbench/full/ukbench00000.jpg", cv::IMREAD_COLOR);
		std::string xd = dbop::serializeMat(ima.getImageMat());
		cv::Mat woah = dbop::deserializeMat(xd);
		

		/*std::vector<img::Image> asd = img::readImageFolder("C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/ukbench/full/", cv::IMREAD_COLOR, false, 1);
		gen::tout << ima.getImageMat().rows * ima.getImageMat().cols << std::endl;
		gen::tout << ima.getImageName() << std::endl;
		std::vector<gen::CompareTest> xdc;
		for (int i = 0; i < asd.size(); i++) {
			float xd = iop::calculateCosineSimilarity(asd[i], ima, 10, 4);
			gen::CompareTest xdf(&ima, &asd[i], xd);
			xdc.push_back(xdf);
		}
		std::sort(xdc.begin(), xdc.end(), gen::compareBySimilarityValue);
		for (auto a : xdc) {
			gen::tout << a.imgrh->getImageName() << "\t" << a.simval << std::endl;
		}*/
		cv::Mat xde = ima.getImageMat();
		cv::uint8_t* pixelPtr = (uint8_t*)xde.data;
		int cn = xde.channels();
		cv::Scalar_<uint8_t> bgrPixel;

		for (int i = 0; i < xde.rows; i++)
		{
			for (int j = 0; j < xde.cols; j++)
			{
				bgrPixel.val[0] = pixelPtr[i * xde.cols * cn + j * cn + 0]; // B
				bgrPixel.val[1] = pixelPtr[i * xde.cols * cn + j * cn + 1]; // G
				bgrPixel.val[2] = pixelPtr[i * xde.cols * cn + j * cn + 2]; // R
			}
		}
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
