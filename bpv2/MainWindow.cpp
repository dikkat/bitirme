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
	QImage imgIn = QImage((uchar*)operand.data, operand.cols, operand.rows, operand.step, QImage::Format_BGR888);
	return imgIn;
}

TableModel::TableModel(QObject* parent)	: QSqlRelationalTableModel(parent) {}

QVariant TableModel::data(const QModelIndex& index, int role) const {
	int col = index.column();
	if (col == 0) {
		if (role == Qt::DecorationRole) {
			QString imhash = QSqlRelationalTableModel::data(index.sibling(index.row(), fieldIndex("hash"))).toString();
			std::string condition = "imhash='" + imhash.toStdString() + "'";

			std::vector<std::vector<std::string>> hashVec = mw_dbPtr->select_GENERAL(
				std::vector<std::vector<std::string>>{ {"iconhash"}, { "imageicon" }, { condition }});

			bool check = true;		
			if (hashVec[0].size() == 0) {
				check = false;
			}
			else if (hashVec[0].size() == 1) {
				std::string condition = "hash='" + hashVec[0][0] + "'";

				std::vector<std::vector<std::string>> hashVec_inner = mw_dbPtr->select_GENERAL(
					std::vector<std::vector<std::string>>{ {"mat"}, { "icon" }, { condition }});
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
		
		std::string condition = "srchash='" + srchash.toStdString() + "'";

		std::vector<std::vector<std::string>> hashVec = mw_dbPtr->select_GENERAL(
			std::vector<std::vector<std::string>>{ {"desthash"}, { "similarity" }, { condition }});

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
	

	QRegExpValidator* validator = new QRegExpValidator(QRegExp("[0-9]"), this);
	ui.lineEdit_alpha->setValidator(validator);
	ui.lineEdit_gauss->setValidator(validator);
	ui.lineEdit_radius->setValidator(validator);
	ui.lineEdit_sigma->setValidator(validator);
	ui.lineEdit_sigmad->setValidator(validator);
	ui.lineEdit_sigmai->setValidator(validator);
	ui.lineEdit_squareSize->setValidator(validator);
	ui.lineEdit_thigh->setValidator(validator);
	ui.lineEdit_tlow->setValidator(validator);

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
	MWObject.connect(ui.imageLabelButton_2, SIGNAL(clicked()), this, SLOT(openImageLabel()));
	MWObject.connect(ui.pushButton_disp, SIGNAL(clicked()), this, SLOT(displayFeature()));
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

void MainWindow::displayHash(img::Image* dest_img) {
	if (srcImg == nullptr) {
		throw std::exception("Unable to locate source image. Load image to source first.");
	}
	if(dest_img != nullptr)
		ui.label_imHash->setText(std::to_string(srcImg->getHash()).c_str());
	else
		ui.label_destHash->setText(std::to_string(dest_img->getHash()).c_str());
}

void MainWindow::displayFeature() {
	QImage feat_img = calculateFeature(ui.tabWidget_comparison->currentIndex());
	ui.label_imgDest->setPixmap(QPixmap::fromImage(feat_img));
}

//fbin, sbin, tbin, flag
QList<int> MainWindow::prepareHistogram() {
	QList<int> histVals;

	histVals.push_back(ui.comboBox_histFlag->currentIndex());
	histVals.push_back(ui.horizontalSlider_fbin->value());
	histVals.push_back(ui.horizontalSlider_sbin->value());
	histVals.push_back(ui.horizontalSlider_tbin->value());
	
	//if it is not gray hist and any of the sliders are zero OR if it is gray and first slider is zero ERROR
	if ((histVals[3] != 0 && (histVals[0] == 0 || histVals[1] == 0 || histVals[2] == 0)) || (histVals[3] == 0 && histVals[0] == 0))
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

	if (cornerVals[1] == 0 || cornerVals[2] == 0 || cornerVals[3] == 0 || cornerVals[4] == 0 || cornerVals[5] == 0)
		throw std::exception("Zero value boxes. Harris values can't be zero, pick a value.");

	return cornerVals;
}

QImage MainWindow::calculateFeature(int index) {
	if (index == 0) { 
		feat::Histogram srcImg_hist(QImageToCvMat(ui.label_imgSrc->pixmap()->toImage()), ui.comboBox_histFlag->currentIndex());
		gen::imageTesting(srcImg_hist.createHistogramDisplayImage(300, 300), "tester1");
		return cvMatToQImage(srcImg_hist.createHistogramDisplayImage(300, 300));
	}
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
	//imageList[0].setImageDirectory(fileName.toStdString());
	setWindowFilePath(fileName);
	srcImg = &lnkr::createImage(QDir::cleanPath(fileName).toStdString(), cv::IMREAD_COLOR);

	const QString message = tr("Opened \"%1\", %2x%3, Depth: %4")
		.arg(QDir::toNativeSeparators(fileName)).arg(image.width()).arg(image.height()).arg(image.depth());
	statusBar()->showMessage(message);
	
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
	image = newImage;
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