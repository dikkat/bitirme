#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_bpv2.h"

class bpv2 : public QMainWindow
{
	Q_OBJECT

public:
	bpv2(QWidget *parent = Q_NULLPTR);

private:
	Ui::bpv2Class ui;
};
