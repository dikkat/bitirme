#include "MainWindow.h"
#include "TestingOperations.h"
#include <QtWidgets/QApplication>
#include <QDebug>
#include <iostream>
#include <fstream>

int main(int argc, char *argv[])
{
	test::tout << "xawedxawehello world" << std::endl;
	test::printTesting(test::tout);
	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}
