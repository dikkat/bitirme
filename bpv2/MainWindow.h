#pragma once

#include <QMainWindow>
#include "ui_MainWindow.h"

class TextEdit;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = Q_NULLPTR);
private:
	Ui::MainWindow ui;
	TextEdit* textViewer;
	void printToScreen();
};
