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

#include "linker.h"
#include "qcustomplot.h"

extern QSqlDatabase qDB;

class TextEdit;

extern int row;

struct currentFeature {
	currentFeature() {}
	int index = -1;
	union featureUnion {
		featureUnion() {}
		~featureUnion() {}
		feat::Hash hash;
		feat::Histogram histogram;
		feat::Edge edge;
		feat::Corner corner;
	} feature;
	void destroyCurrent();
};

enum class table_type{ TABLE_MAIN, TABLE_RESULT };

QImage cvMatToQImage(const cv::Mat& operand);
cv::Mat QImageToCvMat(const QImage& operand);

class SortProxyModel : public QSortFilterProxyModel {
	Q_OBJECT
public:
	SortProxyModel(QTableView* parent_table, QObject* parent = nullptr);
	QVariant data(const QModelIndex& index, int role) const override;
	void suspendData(std::pair<int, int> interval);
private:
	std::pair<int, int> interval;
	QTableView* parent_table;
};

class TableModel : public QAbstractTableModel {
	Q_OBJECT
public:	
	TableModel(table_type type, std::vector<std::pair<img::Image, float>> imgVec, QObject* parent = nullptr);
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role = 2) override;
	bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
	bool canFetchMore(const QModelIndex& parent) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	void updateComparison(int row, const iop::Comparison comparison);
	void fetchMore(const QModelIndex& parent);
	void insertImage(const img::Image& image);
	void insertComparison(const iop::Comparison comparison);
	table_type getType();
	void setIconColumnHidden(bool hidden);
	std::vector<std::pair<img::Image, float>>* getImgVecPtr();
	void resetTable();
	//void sort(int column, Qt::SortOrder order) override;
private:
	int colCount;
	int rowPos;
	table_type type;
	std::vector<std::pair<img::Image*, float*>> dispVec;
	std::vector<std::pair<img::Image, float>> imgVec;
	QCache<int, cv::Mat>* iconCache;
	bool iconColumnHidden;
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
	void showWarning(QString err);
	Ui::MainWindow getUI();
	~MainWindow();
private slots:
	void customMenuRequested_main(QPoint pos);
	void customMenuRequested_result(QPoint pos);
	void openImageLabel();
	void openList();
	void displayFeature(bool emitted = true, int index = -1);
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
	void statsTab_enableBGR_src(int index);
	void statsTab_enableBGR_dest(int index);
private:
	friend class SortDialog;
	Ui::MainWindow ui;
	iop::FeatureVector* currentfv = nullptr;
	iop::WeightVector* currentwv = nullptr;
	iop::Comparator* comparator = nullptr;
	iop::Comparison* currentcomp = nullptr;
	TableModel* mainModel;
	TableModel* resultModel;
	SortProxyModel* proxyModel_result;
	SortProxyModel* proxyModel_main;
	QString* dir = nullptr;
	QSize* screenSize;
	img::Image* source_img = nullptr;
	img::Image* dest_img = nullptr;
	QString* lastpath = nullptr;
	currentFeature curr_feat_src;
	currentFeature curr_feat_dest;
	int iconIdx, simValIdx = 2;
	void scaleImage(QImage& image, QLabel* imlabel, QWidget* frame);
	void switchDisplayWidgets(bool toLabel, bool source);
	void setIcons();
	QList<int> prepareHistogram();
	QList<float> prepareEdge();
	QList<float> prepareCorner();
	void displayHash(img::Image* src);
	void displayHistogram(img::Image* src, bool source, feat::Histogram* hist = nullptr);
	void displayEdge(img::Image* src, bool source, feat::Edge* edge = nullptr);
	void displayCorner(img::Image* src, bool source, feat::Corner* corner = nullptr);
	void createActions();
	bool loadFiles(const QStringList& fileNames, bool check);
	void setImage(QLabel* imlabel, const QImage& newImage);
	void addToMainTable(img::Image* image);
	void addToMainTableNoRefresh(img::Image* image);
	void refreshTable(TableModel* table);
	void resizeEvent(QResizeEvent* event) override;
	void comparisonOperations(iop::Comparator& comparator);
	void statisticsOperations();
	void displayStatistics();
	void initializeImageFileDialog(QFileDialog& dialog, QFileDialog::AcceptMode acceptMode, bool multipleSelection);
};