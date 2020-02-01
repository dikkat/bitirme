#include "MainWindow.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)	: QMainWindow(parent)
{
	ui.setupUi(this);	
	printToScreen();
}

void MainWindow::printToScreen() {
	QString fileName = "C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/testbridge.txt";
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
		QMessageBox::warning(this, "Warning", "Cannot open file: " + file.errorString());
		return;
	}
	QTextStream in(&file);
	QString text = in.readAll();
	ui.textEdit->setText(text);
	file.close();
}