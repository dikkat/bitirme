#include "MainWindow.h"
#include "GeneralOperations.h"
#include "Image.h"
#include <QtWidgets/QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
	gen::tout << "xawedxawehello world" << std::endl;

	try { 
		img::Image losessomeonedear("C:/ukbench00008.jpg", cv::IMREAD_COLOR);
		losessomeonedear.setImageHist(179, 255);
		losessomeonedear.getImageHist()->setHistogramDisplayImage();

		cv::Mat xde = losessomeonedear.getImageMat();
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
