#pragma once

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QMainWindow>
#include <QDir>
#include <QTableView>
#include <QFileDialog>
#include <QImageReader>
#include <QImageWriter>
#include <QColorSpace>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QHeaderView>
#include <QMimeData>
#include <QScreen>
#include <QObject>
#include <QStandardPaths>
#include <QStatusBar>
#include <QDebug>
#include <QAbstractTableModel>
#include <QFileSystemModel.h>
#include <QtSql/qsqlrelationaltablemodel.h>
#include <QtSql/qsqlerror.h>
#include <QtSql/qsqlquery.h>

#include "ui_MainWindow.h"
#include "iop.h"
#include "dbop.h"
#include "linker.h"

namespace qtdbop {

	const auto INSERT_IMAGE_SQL = QLatin1String(R"(
    insert into image values(?, ?, ?, ?)
    )");

	const auto INSERT_HISTOGRAM_SQL = QLatin1String(R"(
    insert into histogram values(?, ?, ?, ?, ?)
    )");

	const auto INSERT_EDGE_SQL = QLatin1String(R"(
    insert into edge values(?, ?, ?)
    )");

}

extern QSqlDatabase qDB;

class TextEdit;

extern int row;

QImage cvMatToQImage(cv::Mat& operand);
cv::Mat QImageToCvMat(const QImage& operand);

class TableModel : public QSqlRelationalTableModel {
	Q_OBJECT
public:
	TableModel(QObject* parent = nullptr);
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
};

static dbop::Database* mw_dbPtr;

class MainWindow : public QMainWindow , public QObject {
	Q_OBJECT
public:
	MainWindow(dbop::Database dbObj, QWidget *parent = Q_NULLPTR);
	void showError(QString err);
	void showError(const QSqlError& err);
	Ui::MainWindow getUI();
private slots:
	void openImageLabel();
	void hideConsole();
	void openList();
	void displayHash(img::Image* dest_img = nullptr);
	void displayFeature();
private:
	Ui::MainWindow ui;
	QSqlRelationalTableModel* model;
	int iconIdx, simValIdx = 2;
	QList<int> prepareHistogram();
	QList<float> prepareEdge();
	QList<float> prepareCorner();
	QImage calculateFeature(int index);
	void createActions();
	bool loadFiles(const QStringList& fileNames);
	bool loadFile(const QString& fileName);
	void printToScreen(QString fileName, bool file);
	void printToScreen(QString text);
	void setImage(QLabel* imlabel, const QImage& newImage);
	void addToMainTable(img::Image* image);
	QImage image;
	img::Image* srcImg = nullptr;
};