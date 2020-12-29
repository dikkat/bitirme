#include "MainWindow.h"

int row = 0;

QSqlDatabase qDB = QSqlDatabase();

cv::Mat QImageToCvMat(const QImage& operand) {
	cv::Mat mat = cv::Mat(operand.height(), operand.width(), CV_8UC4, (uchar*)operand.bits(), operand.bytesPerLine());
	cv::Mat result = cv::Mat(mat.rows, mat.cols, CV_8UC3);
	int from_to[] = { 0,0,  1,1,  2,2 };
	cv::mixChannels(&mat, 1, &result, 1, from_to, 3);
	return result;
}

QImage cvMatToQImage(cv::Mat& operand) {
	QImage imgIn;
	if (operand.channels() == 3)
		imgIn = QImage((uchar*)operand.data, operand.cols, operand.rows, operand.step, QImage::Format_BGR888);
	else if (operand.channels() == 1) {
		cv::imwrite("buffer.jpg", operand);
		imgIn = QImage("buffer.jpg");
	}
	else
		throw std::exception("Number of image channels can't be different than 1 and 3.");
	return imgIn;
}

TableModel::TableModel(QObject* parent)	: QSqlRelationalTableModel(parent) {}

QVariant TableModel::data(const QModelIndex& index, int role) const {
	int col = index.column();
	if (col == 0) {
		if (role == Qt::DecorationRole) {
			QString imhash = QSqlRelationalTableModel::data(index.sibling(index.row(), fieldIndex("hash"))).toString();
			string condition = "imhash='" + imhash.toStdString() + "'";

			std::vector<std::vector<string>> hashVec = mw_dbPtr->select_GENERAL(
				std::vector<std::vector<string>>{ {"iconhash"}, { "imageicon" }, { condition }});

			bool check = true;		
			if (hashVec[0].size() == 0) {
				check = false;
			}
			else if (hashVec[0].size() == 1) {
				string condition = "hash='" + hashVec[0][0] + "'";

				std::vector<std::vector<string>> hashVec_inner = mw_dbPtr->select_GENERAL(
					std::vector<std::vector<string>>{ {"mat"}, { "icon" }, { condition }});
				if (hashVec_inner.size() == 0) {
					check = false;
				}
				else if (hashVec_inner[0].size() == 1) {
					img::Icon imgIcon(dbop::deserializeMat(hashVec_inner[0][0]));
					return QIcon(QPixmap::fromImage(cvMatToQImage(imgIcon.getIconMat())));
				}
			}
			if (!check) {
				QString imgdir = QSqlRelationalTableModel::data(index.sibling(index.row(), fieldIndex("dir"))).toString();
				img::Icon imgIcon(cv::imread(imgdir.toStdString(), cv::IMREAD_COLOR));
				return QIcon(QPixmap::fromImage(cvMatToQImage(imgIcon.getIconMat())));
			}
		}
	}
	else if (col == 1) {
		return QSqlRelationalTableModel::data(index.sibling(index.row(), fieldIndex("name")), role);
	}
	else if (col == 2) {
		QString srchash = QSqlRelationalTableModel::data(index.sibling(index.row(), fieldIndex("hash"))).toString();
		
		string condition = "srchash='" + srchash.toStdString() + "'";

		std::vector<std::vector<string>> hashVec = mw_dbPtr->select_GENERAL(
			std::vector<std::vector<string>>{ {"desthash"}, { "similarity" }, { condition }});

		if (hashVec[0].size() == 0) {
			return QVariant(QString("N/A"));
		}
		else {
			return QString(hashVec[0][0].c_str());
		}
	}
	else
		return QSqlRelationalTableModel::data(index, role);
	return QVariant();
}

//void TableModel::populateData(std::vector<img::Image> data) {
//	columnImage.clear();
//	for (int i = 0; i < data.size(); i++) {
//		//columnImage.push_back(data[i]);
//		//std::cout << modelData[i]->getImageMat().data;
//	}
//	return;
//}
//
//void TableModel::insertImage(img::Image* data) {
//	columnName.push_back(QString(data->getImageName().c_str()));
//	columnSimilarity.push_back("N/A");
//}

MainWindow::MainWindow(dbop::Database dbObj, QWidget *parent)	: QMainWindow(parent) {
	ui.setupUi(this);

	mw_dbPtr = &dbObj;

	qDB = QSqlDatabase::addDatabase("QSQLITE");
	qDB.setDatabaseName("C:/Users/ASUS/source/repos/bpv2/bpv2/bitirme.db");

	if (!qDB.open())
		showError(qDB.lastError());

	TableModel* model = new TableModel(ui.mainTableView);

	model->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model->setTable("image");
	model->setHeaderData(0, Qt::Horizontal, tr("Thumbnail"), Qt::DecorationRole);
	model->setHeaderData(1, Qt::Horizontal, tr("Name"), Qt::DisplayRole);
	model->setHeaderData(2, Qt::Horizontal, tr("Similarity"), Qt::DisplayRole);
	
	ui.mainTableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	ui.mainTableView->verticalHeader()->setDefaultSectionSize(100);
	ui.mainTableView->verticalHeader()->setVisible(false);

	ui.mainTableView->setModel(model);
	ui.mainTableView->horizontalHeader()->setModel(model);
	ui.mainTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	ui.mainTableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
	ui.mainTableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);

	ui.label_imgSrc->setBackgroundRole(QPalette::Base);
	ui.label_imgSrc->setScaledContents(true);

	ui.textEdit->hide();

	QDoubleValidator* validator = new QDoubleValidator(0.0001, 100.0, 1000);
	ui.lineEdit_alpha->setValidator(validator);
	ui.lineEdit_alpha->setText("0.04");
	ui.lineEdit_gauss->setValidator(validator);
	ui.lineEdit_gauss->setText("31");
	ui.lineEdit_numOfScales->setValidator(validator);
	ui.lineEdit_numOfScales->setText("3");
	ui.lineEdit_radius->setValidator(validator);
	ui.lineEdit_radius->setText("3");
	ui.lineEdit_sigma->setValidator(validator);
	ui.lineEdit_sigma->setText("1.4");
	ui.lineEdit_sigmad->setValidator(validator);
	ui.lineEdit_sigmad->setText("0.9");
	ui.lineEdit_sigmai->setValidator(validator);
	ui.lineEdit_sigmai->setText("1.4");
	ui.lineEdit_squareSize->setValidator(validator);
	ui.lineEdit_squareSize->setText("3");
	ui.lineEdit_thigh->setValidator(validator);
	ui.lineEdit_thigh->setText("0.13");
	ui.lineEdit_tlow->setValidator(validator);
	ui.lineEdit_tlow->setText("0.075");

	ui.checkBox_B->setVisible(false);
	ui.checkBox_G->setVisible(false);
	ui.checkBox_R->setVisible(false);

	ui.horizontalSlider_sbin->setEnabled(false);
	ui.horizontalSlider_tbin->setEnabled(false);

	ui.stackedWidget_src->setCurrentIndex(0);
	ui.stackedWidget_dest->setCurrentIndex(0);
	
	img::Image srcImg("C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/ukbench00140.jpg", cv::IMREAD_COLOR);
	ui.label_imgSrc->setPixmap(QPixmap::fromImage(cvMatToQImage(
		srcImg.getImageMat())).scaled(ui.label_imgSrc->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
	lnkr::setSourceImage(srcImg);

	ui.gridLayout_9->setAlignment(ui.verticalSlider_TL, Qt::AlignHCenter);
	ui.gridLayout_10->setAlignment(ui.verticalSlider_TR, Qt::AlignHCenter);
	ui.gridLayout_6->setAlignment(ui.verticalSlider_BL, Qt::AlignHCenter);
	ui.gridLayout_11->setAlignment(ui.verticalSlider_BR, Qt::AlignHCenter);

	/*printToScreen("C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/testbridge.txt", true);

	img::Image imb("C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/testImage.jpg", cv::IMREAD_COLOR);
	setImage(ui.imageLabel, cvMatToQImage(imb.getImageMat()));
	
	std::vector<img::Image> xd;
	for (int i = 0; i < 6; i++) {
		img::Image ima("C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/ukbench/full/ukbench0000" + std::to_string(i) + ".jpg", cv::IMREAD_COLOR);
		xd.push_back(ima);
		addToMainTable(&xd[i]);
	}*/

	QObject MWObject;
	MWObject.connect(ui.pushButton_srcImgLabel, SIGNAL(clicked()), this, SLOT(openImageLabel()));
	MWObject.connect(ui.consoleButton, SIGNAL(clicked()), this, SLOT(hideConsole()));
	MWObject.connect(ui.pushButton_dispInDet, SIGNAL(clicked()), this, SLOT(displayFeature()));
	MWObject.connect(ui.comboBox_histFlag, SIGNAL(currentIndexChanged(int)), this, SLOT(displayButtons_BGR()));
}

MainWindow::~MainWindow(){
	mw_dbPtr->delete_GENERAL(std::vector<string>{"sourceimage"});
}

void MainWindow::showError(const QSqlError& err) {
	QMessageBox::critical(this, "Unable to initialize Database",
		"Error initializing database: " + err.text());
}

void MainWindow::showError(QString err) {
	QMessageBox::critical(this, "ERROR or as we intellectuals call it, EXCEPTION", "Error! \n" + err);
}

Ui::MainWindow MainWindow::getUI() {
	return ui;
}

void MainWindow::displayButtons_BGR() {
	if (ui.comboBox_histFlag->currentIndex() == HIST_GRAY) {
		ui.checkBox_B->setVisible(false);
		ui.checkBox_G->setVisible(false);
		ui.checkBox_R->setVisible(false);
		ui.horizontalSlider_sbin->setEnabled(false);
		ui.horizontalSlider_tbin->setEnabled(false);
		ui.horizontalSlider_fbin->setMaximum(255);
		ui.label_bMax->setText("255");
	}
	else if (ui.comboBox_histFlag->currentIndex() == HIST_BGR) {
		ui.checkBox_B->setText("B");
		ui.checkBox_B->setVisible(true);
		ui.checkBox_G->setText("G");
		ui.checkBox_G->setVisible(true);
		ui.checkBox_R->setText("R");
		ui.checkBox_R->setVisible(true);
		ui.horizontalSlider_sbin->setEnabled(true);
		ui.horizontalSlider_tbin->setEnabled(true);
		ui.horizontalSlider_fbin->setMaximum(255);
		ui.label_bMax->setText("255");
	}
	else if (ui.comboBox_histFlag->currentIndex() == HIST_HSV) {
		ui.checkBox_B->setText("H");
		ui.checkBox_B->setVisible(true);
		ui.checkBox_G->setText("S");
		ui.checkBox_G->setVisible(true);
		ui.checkBox_R->setText("V");
		ui.checkBox_R->setVisible(true);
		ui.horizontalSlider_sbin->setEnabled(true);
		ui.horizontalSlider_tbin->setEnabled(true);
		ui.horizontalSlider_fbin->setMaximum(180);
		ui.label_bMax->setText("180");
	}
}

void MainWindow::displayFeature() {
	int index = ui.tabWidget_comparison->currentIndex();
	img::Image srcImg = mw_dbPtr->select_SourceImage();

	if (index == 0) {
		displayHash(&srcImg);
	}
	else if (index == 1) {
		displayHistogram(&srcImg, true);
		ui.mainTabWidget->setCurrentIndex(1);
	}
	else if (index == 2) {
		displayEdge(&srcImg, true);
		ui.mainTabWidget->setCurrentIndex(1);
	}
	else if (index == 3) {
		displayCorner(&srcImg, true);
		ui.mainTabWidget->setCurrentIndex(1);
	}
}

void MainWindow::displayHash(img::Image* src) {
	if (ui.label_imgSrc->pixmap()->isNull()) {
		throw std::exception("Unable to locate source image. Load image to source first.");
	}
	else {
		/*string srcDHash = feat::Hash::imageHashing_dHash(QImageToCvMat(ui.label_imgSrc->pixmap()->toImage())).to_string();
		ui.label_imHash->setText(srcDHash.c_str());*/
	}

	if (!ui.label_imgDest->pixmap()->isNull()) {		
		/*string destDHash = feat::Hash::imageHashing_dHash(QImageToCvMat(ui.label_imgDest->pixmap()->toImage())).to_string();
		ui.label_destHash->setText(destDHash.c_str());*/ //TODO: FIX THESE
	}
	else {
		throw std::exception("Unable to locate comparison image. Load image to compare first.");
	}
}

void MainWindow::displayHistogram(img::Image* src, bool source){
	QList<int> histVals;
	try {
		histVals = prepareHistogram();
	}
	catch (...) {
		return; //TODO: HATAYI YAKALA
	}
	feat::Histogram* srcHist = lnkr::setHistogram(src, histVals[0], histVals[1], histVals[2], histVals[3]);
	cv::Mat histPtr = srcHist->getHistogramMat();

	double max;
	cv::minMaxLoc(histPtr, nullptr, &max);

	QCustomPlot* plotSrc;
	if (source)
		plotSrc = ui.plot_source;
	else
		plotSrc = ui.plot_dest;

	plotSrc->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
	plotSrc->legend->setVisible(true);
	QFont legendFont = font();
	legendFont.setPointSize(6);
	plotSrc->legend->setFont(legendFont);
	plotSrc->legend->setBrush(QBrush(QColor(255, 255, 255, 230)));
	plotSrc->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignRight);
	plotSrc->axisRect()->insetLayout()->setMargins(QMargins(0, 2, 2, 0));
	plotSrc->legend->setIconSize(QSize(10, 15));

	if (histPtr.channels() == 1) {
		QVector<double> data, keys;
		for (int i = 0; i < histPtr.total(); i++) {
			keys.push_back(static_cast<float>(255) / static_cast<float>(histPtr.total()) * i);
			data.push_back(static_cast<double>(histPtr.at<float>(i)));
		}

		int pc = plotSrc->plottableCount();
		for (int i = 0; i < pc; i++) {
			plotSrc->removeGraph(plotSrc->graph(0));
			plotSrc->removePlottable(plotSrc->plottable(0));
		}
		plotSrc->replot();

		//QCPBars* barSrc = new QCPBars(plotSrc->xAxis, plotSrc->yAxis); TODO: BAR OR GRAPH DECIDE
		QCPGraph* graphPtr = new QCPGraph(plotSrc->xAxis, plotSrc->yAxis);
		graphPtr->setData(keys, data);
		graphPtr->setName("Gray Histogram");
		graphPtr->setPen(QPen(QColor(Qt::lightGray)));
		plotSrc->xAxis->setLabel("Histogram bins");
		plotSrc->yAxis->setLabel("Number of pixels");
		plotSrc->yAxis->setRange(0, max + max * 0.10);
		plotSrc->xAxis->setRange(0, 255 + 255 * 0.10);
		plotSrc->replot();
	}

	else if (histPtr.channels() == 3 && srcHist->getVariablesFloat()[0] == HIST_BGR) {
		QVector<QVector<double>> data;
		data.resize(3);
		QVector<double> keys;
		for (int i = 0; i < histPtr.total(); i++) {
			keys.push_back(i);
			data[0].push_back(static_cast<double>(histPtr.at<cv::Vec3f>(i)[0]));
			data[1].push_back(static_cast<double>(histPtr.at<cv::Vec3f>(i)[1]));
			data[2].push_back(static_cast<double>(histPtr.at<cv::Vec3f>(i)[2]));
		}

		int pc = plotSrc->plottableCount();
		for (int i = 0; i < pc; i++) {
			plotSrc->removeGraph(plotSrc->graph(0));
			plotSrc->removePlottable(plotSrc->plottable(0));
		}

		plotSrc->replot();

		if (ui.checkBox_B->checkState()) {
			QCPGraph* graphPtrB = new QCPGraph(plotSrc->xAxis, plotSrc->yAxis);
			//QCPBars* barSrcB = new QCPBars(plotSrc->xAxis, plotSrc->yAxis);
			graphPtrB->setPen(QPen(QColor(Qt::darkBlue)));
			graphPtrB->setData(keys, data[0]);
			graphPtrB->setName("Blue Histogram");
		}

		if (ui.checkBox_G->checkState()) {
			QCPGraph* graphPtrG = new QCPGraph(plotSrc->xAxis, plotSrc->yAxis);
			//QCPBars* barSrcG = new QCPBars(plotSrc->xAxis, plotSrc->yAxis);
			graphPtrG->setPen(QPen(QColor(Qt::darkGreen)));
			graphPtrG->setData(keys, data[1]);
			graphPtrG->setName("Green Histogram");
		}

		if (ui.checkBox_R->checkState()) {
			QCPGraph* graphPtrR = new QCPGraph(plotSrc->xAxis, plotSrc->yAxis);
			//QCPBars* barSrcR = new QCPBars(plotSrc->xAxis, plotSrc->yAxis);
			graphPtrR->setPen(QPen(QColor(Qt::darkRed)));
			graphPtrR->setData(keys, data[2]);
			graphPtrR->setName("Red Histogram");
		}

		plotSrc->xAxis->setLabel("Histogram bins");
		plotSrc->yAxis->setLabel("Number of pixels");
		plotSrc->yAxis->setRange(0, max + max * 0.10);
		plotSrc->xAxis->setRange(0, histPtr.rows + histPtr.rows * 0.10);
		plotSrc->replot();
	}

	else if (histPtr.channels() == 3 && srcHist->getVariablesFloat()[0] == HIST_HSV) {
		QVector<QVector<double>> data;
		data.resize(3);
		QVector<double> keys;
		for (int i = 0; i < histPtr.total(); i++) {
			keys.push_back(i);
			data[0].push_back(static_cast<double>(histPtr.at<cv::Vec3f>(i)[0]));
			data[1].push_back(static_cast<double>(histPtr.at<cv::Vec3f>(i)[1]));
			data[2].push_back(static_cast<double>(histPtr.at<cv::Vec3f>(i)[2]));
		}

		int pc = plotSrc->plottableCount();
		for (int i = 0; i < pc; i++) {
			plotSrc->removeGraph(plotSrc->graph(0));
			plotSrc->removePlottable(plotSrc->plottable(0));
		}

		plotSrc->replot();
		if (ui.checkBox_B->checkState()) {
			QCPGraph* graphPtrH = new QCPGraph(plotSrc->xAxis, plotSrc->yAxis);
			//QCPBars* barSrcH = new QCPBars(plotSrc->xAxis, plotSrc->yAxis);
			graphPtrH->setPen(QPen(QColor(Qt::darkMagenta)));
			graphPtrH->setData(keys, data[0]);
			graphPtrH->setName("Hue Histogram");
		}

		if (ui.checkBox_G->checkState()) {
			QCPGraph* graphPtrS = new QCPGraph(plotSrc->xAxis, plotSrc->yAxis);
			//QCPBars* barSrcS = new QCPBars(plotSrc->xAxis, plotSrc->yAxis);
			graphPtrS->setPen(QPen(QColor(Qt::darkCyan)));
			graphPtrS->setData(keys, data[1]);
			graphPtrS->setName("Saturation Histogram");
		}
		
		if (ui.checkBox_R->checkState()) {
			QCPGraph* graphPtrV = new QCPGraph(plotSrc->xAxis, plotSrc->yAxis);
			//QCPBars* barSrcV = new QCPBars(plotSrc->xAxis, plotSrc->yAxis);
			graphPtrV->setPen(QPen(QColor(Qt::darkGray)));
			graphPtrV->setData(keys, data[2]);
			graphPtrV->setName("Value Histogram");
		}

		plotSrc->xAxis->setLabel("Histogram bins");
		plotSrc->yAxis->setLabel("Number of pixels");
		plotSrc->yAxis->setRange(0, max + max * 0.10);
		plotSrc->xAxis->setRange(0, histPtr.rows + histPtr.rows * 0.10);
		plotSrc->replot();
	}

	delete(srcHist);
}

void MainWindow::displayEdge(img::Image* src, bool source) {
	QList<float> edgeVals = prepareEdge();
	feat::Edge* srcEdge;

	if (edgeVals[0] == EDGE_CANNY) {
		cv::Mat kernelX;
		cv::Mat kernelY;
		switch (static_cast<int>(edgeVals[5])) {
		case KERNEL_SOBEL:
			kernelX = feat::sobelX;
			kernelY = feat::sobelY;
			break;
		case KERNEL_PREWT:
			kernelX = feat::prewittX;
			kernelY = feat::prewittY;
			break;
		case KERNEL_ROBRT:
			kernelX = feat::robertX;
			kernelY = feat::robertY;
			break;
		default:
			throw std::exception("Illegal kernel flag.");
			break;
		}
		feat::Edge::Canny* canny = new feat::Edge::Canny(edgeVals[1], edgeVals[4], edgeVals[2], edgeVals[3], kernelX, kernelY);
		srcEdge = lnkr::setEdge(src, edgeVals[0], canny);
	}

	else {
		srcEdge = lnkr::setEdge(src, edgeVals[0]);
	}

	if (source) {
		switchDisplayWidgets(true, true);
		ui.label_derSrcBig->setPixmap(QPixmap::fromImage(QImage(cvMatToQImage(srcEdge->getEdgeMat()))));
	}
	else {
		switchDisplayWidgets(true, false);
		ui.label_derDestBig->setPixmap(QPixmap::fromImage(QImage(cvMatToQImage(srcEdge->getEdgeMat()))));
	}

	delete(srcEdge);
}
//cornerFlag, radius, squareSize, alpha, sigmai, sigmad
void MainWindow::displayCorner(img::Image* src, bool source) {
	QList<float> cornerVals = prepareCorner();
	feat::Corner* srcCorner;

	feat::Corner::Harris* harris = new feat::Corner::Harris(cornerVals[1], cornerVals[2], cornerVals[4], cornerVals[5], cornerVals[3]);
	srcCorner = lnkr::setCorner(src, *harris, cornerVals[0], cornerVals[6], 0);

	if (source) {
		switchDisplayWidgets(true, true);
		ui.label_derSrcBig->setPixmap(QPixmap::fromImage(QImage(cvMatToQImage(srcCorner->getCornerMarkedMat()))));
	}
	else {
		switchDisplayWidgets(true, false);
		ui.label_derDestBig->setPixmap(QPixmap::fromImage(QImage(cvMatToQImage(srcCorner->getCornerMarkedMat()))));
	}

	delete(srcCorner);
}

void MainWindow::switchDisplayWidgets(bool toLabel, bool source) {
	if (toLabel) {
		if (source)
			ui.stackedWidget_src->setCurrentIndex(1);

		else if (!source)
			ui.stackedWidget_dest->setCurrentIndex(1);
	}
	else {
		if (source)
			ui.stackedWidget_src->setCurrentIndex(0);

		else if (!source)
			ui.stackedWidget_dest->setCurrentIndex(0);
	}
}

//flag, fbin, sbin, tbin
QList<int> MainWindow::prepareHistogram() {
	QList<int> histVals;

	histVals.push_back(ui.comboBox_histFlag->currentIndex());
	histVals.push_back(ui.horizontalSlider_fbin->value());
	histVals.push_back(ui.horizontalSlider_sbin->value());
	histVals.push_back(ui.horizontalSlider_tbin->value());
	
	//if it is not gray hist and any of the sliders are zero OR if it is gray and first slider is zero ERROR
	if ((histVals[0] != 0 && (histVals[1] == 0 || histVals[2] == 0 || histVals[3] == 0)) 
		|| (histVals[0] == 0 && histVals[1] == 0))
		throw std::exception("Zero value sliders. Sliders can't be zero, pick a value.");

	return histVals;
}

//flag, gauss, tlow, thigh, sigma
QList<float> MainWindow::prepareEdge() {
	QList<float> edgeVals;

	edgeVals.push_back(ui.comboBox_edgeFlag->currentIndex());

	if (ui.comboBox_edgeFlag->currentIndex() == EDGE_CANNY) {
		edgeVals.push_back(ui.lineEdit_gauss->text().toFloat());
		edgeVals.push_back(ui.lineEdit_tlow->text().toFloat());
		edgeVals.push_back(ui.lineEdit_thigh->text().toFloat());
		edgeVals.push_back(ui.lineEdit_sigma->text().toFloat());
		edgeVals.push_back(ui.comboBox_kernelFlag->currentIndex());
	}

	if (edgeVals[0] == EDGE_CANNY && (edgeVals[1] == 0 || edgeVals[2] == 0 || edgeVals[3] == 0))
		throw std::exception("Zero value boxes. Canny values can't be zero, pick a value.");

	return edgeVals;
}

//cornerFlag, radius, squareSize, alpha, sigmai, sigmad
QList<float> MainWindow::prepareCorner() {
	QList<float> cornerVals;

	cornerVals.push_back(ui.comboBox_cornerFlag->currentIndex());
	cornerVals.push_back(ui.lineEdit_radius->text().toFloat());
	cornerVals.push_back(ui.lineEdit_squareSize->text().toFloat());
	cornerVals.push_back(ui.lineEdit_alpha->text().toFloat());
	cornerVals.push_back(ui.lineEdit_sigmai->text().toFloat());
	cornerVals.push_back(ui.lineEdit_sigmad->text().toFloat());
	cornerVals.push_back(ui.lineEdit_numOfScales->text().toFloat());

	if (cornerVals[1] == 0 || cornerVals[2] == 0 || cornerVals[3] == 0 || cornerVals[4] == 0 || cornerVals[5] == 0
		|| cornerVals[6] == 0)
		throw std::exception("Zero value boxes. Harris values can't be zero, pick a value.");

	return cornerVals;
}

void MainWindow::printToScreen(QString fileName, bool filecheck) {
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
		QMessageBox::warning(this, "Warning", "Cannot open file: " + file.errorString());
		return;
	}
	QTextStream in(&file);
	QString text = in.readAll();
	ui.textEdit->append(text);
	file.close();
}

void MainWindow::printToScreen(QString text) {
	ui.textEdit->append(text);
}

bool MainWindow::loadFiles(const QStringList& fileNames) {
	std::vector<img::Image> imgVec;
	for (int i = 0; i < fileNames.size(); i++) {
		QImageReader reader(fileNames[i]);
		reader.setAutoTransform(true);
		const QImage buffer = reader.read();
		img::Image newImage(QDir::toNativeSeparators(fileNames[i]).toStdString(),cv::IMREAD_COLOR);
		if (buffer.isNull()) {
			QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
				tr("Cannot load %1: %2")
				.arg(QDir::toNativeSeparators(fileNames[i]), reader.errorString()));
			continue;
		}
		imgVec.push_back(newImage);
	}
	for (img::Image iter : imgVec)
		addToMainTable(&iter);
	return true;
}

bool MainWindow::loadFile(const QString& fileName)
{
	QImageReader reader(fileName);
	reader.setAutoTransform(true);
	const QImage newImage = reader.read();
	if (newImage.isNull()) {
		QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
			tr("Cannot load %1: %2")
			.arg(QDir::toNativeSeparators(fileName), reader.errorString()));
		return false;
	}

	setImage(ui.label_imgSrc, newImage);
	setWindowFilePath(fileName);
	lnkr::setSourceImage(QDir::cleanPath(fileName).toStdString(), cv::IMREAD_COLOR);

	return true;
}

static void initializeImageFileDialog(QFileDialog& dialog, QFileDialog::AcceptMode acceptMode)
{
	static bool firstDialog = true;
	if (firstDialog) {
		firstDialog = false;
		const QString picturesLocations = "C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/ukbench/full";		
		dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations);
	}
	dialog.setFileMode(QFileDialog::ExistingFile);

	QStringList mimeTypeFilters;
	const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen
		? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
	for (const QByteArray& mimeTypeName : supportedMimeTypes)
		mimeTypeFilters.append(mimeTypeName);
	mimeTypeFilters.sort();
	dialog.setMimeTypeFilters(mimeTypeFilters);
	dialog.selectMimeTypeFilter("image/jpeg");
	if (acceptMode == QFileDialog::AcceptSave)
		dialog.setDefaultSuffix("jpg");
}

static void initializeImageFileDialog(QFileDialog& dialog, QFileDialog::AcceptMode acceptMode, bool multipleSelection) {
	initializeImageFileDialog(dialog, acceptMode);
	dialog.setFileMode(QFileDialog::ExistingFiles);
}

void MainWindow::openList() {
	QFileDialog dialog(this, tr("Open Files"));
	initializeImageFileDialog(dialog, QFileDialog::AcceptOpen, true);

	while (dialog.exec() == QDialog::Accepted && !loadFiles(dialog.selectedFiles())) {}
}

void MainWindow::openImageLabel()
{
	QFileDialog dialog(this, tr("Open File"));
	initializeImageFileDialog(dialog, QFileDialog::AcceptOpen);

	while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first())) {}
}

void MainWindow::hideConsole() {
	if (ui.textEdit->isHidden())
		ui.textEdit->show();
	else
		ui.textEdit->hide(); 
}

void MainWindow::setImage(QLabel* imlabel, const QImage& newImage) {
	QImage image = newImage;
	/*if (imageList.size() == 0) {
		imageList.push_back(QImageToimgImage(image));
	}
	else
		imageList[0] = QImageToimgImage(image);
	std::cout << imageList.size();*/
	if (image.colorSpace().isValid())
		image.convertToColorSpace(QColorSpace::SRgb);
	imlabel->setMaximumHeight(MIN(imlabel->maximumWidth() * image.height() / image.width(), imlabel->maximumWidth()));
	//imlabel->setMinimumHeight(MIN(imlabel->maximumWidth() * image.height() / image.width(), imlabel->maximumWidth()));
	imlabel ->setPixmap(QPixmap::fromImage(image));
}

void MainWindow::addToMainTable(img::Image* image) {
}



void MainWindow::createActions()
//! [17] //! [18]
{
	
}