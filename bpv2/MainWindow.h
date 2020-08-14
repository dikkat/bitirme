#pragma once

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

#include "ui_MainWindow.h"
#include "iop.h"

class TextEdit;

const int COLS = 3;
const int ROWS = 2;

QImage imgImageToQImage(img::Image& operand, int format);
img::Image QImageToimgImage(const QImage& operand);

class TableModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	TableModel(QObject* parent = nullptr);
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	void populateData(std::vector<img::Image> data);
private:
	QList<img::Image*> modelData;  //holds text entered into QTableView
};

class MainWindow : public QMainWindow , public QObject
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = Q_NULLPTR);
private slots:
	void openImageLabel();
	void hideConsole();
	void openList();
private:
	Ui::MainWindow ui;
	void createActions();
	bool loadFiles(const QStringList& fileNames);
	bool loadFile(const QString& fileName);
	void printToScreen(QString fileName, bool file);
	void printToScreen(QString text);
	void setImage(QLabel* imlabel, const QImage& newImage);
	void addToList(QListWidget* list, const QImage& image);

	QImage image;
};
