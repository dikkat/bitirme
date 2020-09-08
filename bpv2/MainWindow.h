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

namespace qtdbop {

	const auto IMAGE_SQL = QLatin1String(R"(
    create table image(hash text primary key not null, iconmat blob not null, name text not null, dir text not null)
    )");

	const auto HISTOGRAM_SQL = QLatin1String(R"(
    create table histogram(hash primary key not null, flag int not null, fbin int not null, sbin int, tbin int)
    )");

	const auto EDGECANNY_SQL = QLatin1String(R"(
    create table edgecanny(hash text primary key not null, gausskernelsize int not null, sigma real not null, thigh real not null,
	tlow real not null, kernelx blob not null, kernely blob not null)
    )");

	const auto EDGE_SQL = QLatin1String(R"(
	create table edge(hash text primary key not null, flag int not null, edc_hash text, foreign key(edc_hash) references 
	edgecanny(hash) on delete cascade)
	)");

	const auto CORNERHARRIS_SQL = QLatin1String(R"(
    create table cornerharris(hash text primary key not null, radius int not null, squaresize int not null, sigmai real not null,
	sigmad real not null, alpha real not null, kernelx blob not null, kernely blob not null)
    )");

	const auto CORNER_SQL = QLatin1String(R"(
	create table corner(hash text primary key not null, flag int not null, cdh_hash text, numberofscales int not null,
	scaleratio real, foreign key(cdh_hash) references cornerharris(hash) on delete cascade)
	)");

	const auto IMAGEHISTOGRAM_SQL = QLatin1String(R"(
	create table image_histogram(img_hash text not null, hist_hash text not null, 
	foreign key(img_hash) references image(hash) on delete cascade,
	foreign key(hist_hash) references histogram(hash) on delete cascade,
	primary key(img_hash,hist_hash))
	)");

	const auto IMAGEEDGE_SQL = QLatin1String(R"(
	create table image_edge(img_hash text not null, edge_hash text not null, 
	foreign key(img_hash) references image(hash) on delete cascade,
	foreign key(edge_hash) references edge(hash) on delete cascade,
	primary key(img_hash,edge_hash))
	)");

	const auto IMAGECORNER_SQL = QLatin1String(R"(
	create table image_corner(img_hash text not null, corner_hash text not null, 
	foreign key(img_hash) references image(hash) on delete cascade,
	foreign key(corner_hash) references corner(hash) on delete cascade,
	primary key(img_hash,corner_hash))
	)");

	const auto INSERT_IMAGE_SQL = QLatin1String(R"(
    insert into image values(?, ?, ?, ?)
    )");

	const auto INSERT_HISTOGRAM_SQL = QLatin1String(R"(
    insert into histogram values(?, ?, ?, ?, ?)
    )");

	const auto INSERT_EDGE_SQL = QLatin1String(R"(
    insert into edge values(?, ?, ?)
    )");

	QSqlError initDb();
}

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

class MainWindow : public QMainWindow , public QObject {
	Q_OBJECT
public:
	MainWindow(QWidget *parent = Q_NULLPTR);
private slots:
	void openImageLabel();
	void hideConsole();
	void openList();
private:
	Ui::MainWindow ui;
	QSqlRelationalTableModel* model;
	int iconIdx, simValIdx;
	void showError(const QSqlError& err);
	void createMenuBar();
	void createActions();
	bool loadFiles(const QStringList& fileNames);
	bool loadFile(const QString& fileName);
	void printToScreen(QString fileName, bool file);
	void printToScreen(QString text);
	void setImage(QLabel* imlabel, const QImage& newImage);
	void addToMainTable(img::Image* image);

	QImage image;
};
