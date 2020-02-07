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
