#include "MainWindow.h"
#include "GeneralOperations.h"
#include "ImageReader.h"
#include <QtWidgets/QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
	gen::tout << "xawedxawehello world" << std::endl;
	gen::printTesting(gen::tout);
	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}
