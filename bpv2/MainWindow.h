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
#include "ui_DialogDetail.h"

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
	//void sort(int column, Qt::SortOrder order) override;
};

static dbop::Database* mw_dbPtr;

class MainWindow;

class DetailDialog : public QDialog {
	Q_OBJECT
public:
	DetailDialog(QWidget* parent, iop::Comparison* comp = nullptr, iop::WeightVector* wv = nullptr);
private:
	Ui::DialogDetail ui;
	MainWindow* parent_ptr = nullptr;
	iop::Comparison* current_comp;
};

class SortDialog : public QDialog {
	Q_OBJECT
public:
	SortDialog(QWidget* parent = Q_NULLPTR, iop::FeatureVector* fv = nullptr, iop::WeightVector* wv = nullptr);
private:
	Ui::DialogSort ui;
	MainWindow* parent_ptr = nullptr;
	float* current = nullptr;
	std::vector<vecf> binVec;
	std::vector<vecf> weightVec;
	std::vector<bool> enableVec;
	std::vector<std::vector<bool>> bgrEnableVec;
	std::vector<QLineEdit*> initlineedit_vec;
	std::vector<std::vector<QLineEdit*>> lineedit_hist_vec;
	std::vector<QLineEdit*> lineedit_hist_g;
	std::vector<QLineEdit*> lineedit_hist_bgr;
	std::vector<QLineEdit*> lineedit_hist_hsv;
	std::vector<std::vector<QSlider*>> slider_hist_vec;
	std::vector<QSlider*> slider_hist_g;
	std::vector<QSlider*> slider_hist_bgr;
	std::vector<QSlider*> slider_hist_hsv;
	std::vector<QCheckBox*> checkbox_vec;
	std::vector<std::vector<QCheckBox*>> checkbox_vec_bgr;
private slots:
	void buildFeatureVector();
	void remainingPercentage(QString sent, bool disabled = false);
	void enableHash(bool enable);
	void enableHistG(bool enable);
	void enableHistB(bool enable);
	void enableHistH(bool enable);
	void enableEdge(bool enable);
	void displayButtons_BGR(int state);
	void enableBGR(bool enable, int combobox, int index);
	void equaliseWeights(bool enable);
private:
	void initialiseWeights(iop::FeatureVector* fv, iop::WeightVector* wv);
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
	void customMenuRequested_main(QPoint pos);
	void customMenuRequested_result(QPoint pos);
	void openImageLabel();
	void openList();
	void displayFeature();
	void displayButtons_BGR();
	bool loadFile(const QString& fileName, bool source = true);
	void deleteImage(QString& fileName);
	void showIcons(bool show);
	void openSortDialog();
	void openDetailDialog(iop::Comparison* comp, iop::WeightVector* wvec);
	//void enableHistComp(bool enable);
	void enableCanny(int state);
	void setFeatureVector(iop::FeatureVector returnfv, iop::WeightVector returnwv);
	void copyToClipboard(QString& str, bool folder = false);
	void compareMain();
	void switchTables();
private:
	friend class SortDialog;
	Ui::MainWindow ui;
	iop::FeatureVector* currentfv = nullptr;
	iop::WeightVector* currentwv = nullptr;
	iop::Comparator* comparator;
	iop::Comparison* currentcomp = nullptr;
	TableModel* mainModel;
	TableModel* resultModel;
	QSortFilterProxyModel* proxyModel_result;
	QString* dir = nullptr;
	QSize* screenSize;
	img::Image* source_img = nullptr;
	img::Image* dest_img = nullptr;
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
	void refreshTable(TableModel* table);
};