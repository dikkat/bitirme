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
#include "ui_DialogSort.h"
#include "iop.h"
#include "dbop.h"
#include "linker.h"
#include "qcustomplot.h"

extern QSqlDatabase qDB;

class TextEdit;

extern int row;

QImage cvMatToQImage(cv::Mat& operand);
cv::Mat QImageToCvMat(const QImage& operand);

class TableModel : public QSqlQueryModel {
	Q_OBJECT
public:
	TableModel(QObject* parent = nullptr);
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
};

static dbop::Database* mw_dbPtr;

class MainWindow;

class SortDialog : public QDialog {
	Q_OBJECT
public:
	SortDialog(QWidget* parent = Q_NULLPTR);
private:
	Ui::Dialog ui;
	MainWindow *parent_ptr = nullptr;
	float* current = nullptr;
	int* binstate = nullptr;
	std::vector<vecf> binVec;
	std::vector<vecf> weightVec;
	std::vector<bool> enableVec;
	std::vector<std::vector<bool>> bgrEnableVec;
	std::vector<QLineEdit*> lineedit_arr;
	std::vector<QLineEdit*> initlineedit_arr;
	std::vector<QLineEdit*> lineedit_hist_arr;
private slots:
	void buildFeatureVector();
	void remainingPercentage(QString sent, bool disabled = false);
	void enableHash(bool enable);
	void enableHistG(bool enable);
	void enableHistB(bool enable);
	void enableHistH(bool enable);
	void enableEdge(bool enable);
	void displayButtons_BGR(int state);
	void enableBGR(bool enable, int index);
	void equaliseWeights(bool enable);
signals:
	void returnFeatureVector(iop::FeatureVector fv, iop::WeightVector wv);
};

class MainWindow : public QMainWindow , public QObject {
	Q_OBJECT
public:
	MainWindow(dbop::Database dbObj, QWidget *parent = Q_NULLPTR);
	void showError(QString err);
	void showError(const QSqlError& err);
	Ui::MainWindow getUI();
	~MainWindow();
private slots:
	void customMenuRequested(QPoint pos);
	void openImageLabel();
	void openList();
	void displayFeature();
	void displayButtons_BGR();
	bool loadFile(const QString& fileName, bool source = true);
	void deleteImage(QString& fileName);
	void showIcons(bool show);
	void openSortDialog();
	//void enableHistComp(bool enable);
	void enableCanny(int state);
	void setFeatureVector(iop::FeatureVector returnfv, iop::WeightVector returnwv);
	void copyToClipboard(QString& str, bool folder = false);
private:
	friend class SortDialog;
	Ui::MainWindow ui;
	iop::FeatureVector* currentfv = nullptr;
	iop::WeightVector* currentwv = nullptr;
	TableModel* mainModel;
	TableModel* resultModel;
	QString* dir = nullptr;
	QSize* screenSize;
	int iconIdx, simValIdx = 2;
	void scaleImage(QImage& image, QLabel* imlabel, QWidget* frame);
	void switchDisplayWidgets(bool toLabel, bool source);
	void setIcons();
	QList<int> prepareHistogram();
	QList<float> prepareEdge();
	QList<float> prepareCorner();
	void displayHash(img::Image* src);
	void displayHistogram(img::Image* src, bool source);
	void displayEdge(img::Image* src, bool source);
	void displayCorner(img::Image* src, bool source);
	void createActions();
	bool loadFiles(const QStringList& fileNames);
	void setImage(QLabel* imlabel, const QImage& newImage);
	void addToMainTable(img::Image* image);
	void refreshTable(TableModel* table, bool main);
};