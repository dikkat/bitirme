#include "MainWindow.h"

int row = 0;

QSqlError qtdbop::initDb() {
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName("C:/Users/ASUS/source/repos/bpv2/bpv2/bitirme.db");

	if (!db.open())
		return db.lastError();

	QSqlQuery q;
	if (!q.exec(IMAGE_SQL))
		return q.lastError();
	if (!q.exec(HISTOGRAM_SQL))
		return q.lastError();
	if (!q.exec(EDGECANNY_SQL))
		return q.lastError();
	if (!q.exec(EDGE_SQL))
		return q.lastError();
	if (!q.exec(CORNERHARRIS_SQL))
		return q.lastError();
	if (!q.exec(CORNER_SQL))
		return q.lastError();
	if (!q.exec(IMAGEHISTOGRAM_SQL))
		return q.lastError();
	if (!q.exec(IMAGEEDGE_SQL))
		return q.lastError();
	if (!q.exec(IMAGECORNER_SQL))
		return q.lastError();
	return QSqlError();
}

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
	if (index.column() == this->fieldIndex("iconmat")) {
		if (role == Qt::DecorationRole) {
			std::string data = QSqlRelationalTableModel::data(index, Qt::DisplayRole).toString().toStdString();
			cv::Mat dataMat = dbop::deserializeMat(data);
			
			return QIcon(QPixmap::fromImage(cvMatToQImage(dataMat)));
		}
	}
	else
		return QSqlRelationalTableModel::data(index, role);
	// generate a log message when this method gets called
	/*if (role == Qt::DisplayRole) {
		if (row == 0) {
			std::cout << col << modelData.size() << modelData[col]->getImageMat().data << imageList[0].getImageMat().data;
			img::Image xd = *modelData[col];
			return QIcon(QPixmap::fromImage(imgImageToQImage(xd)));
		}
	}*/
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

MainWindow::MainWindow(QWidget *parent)	: QMainWindow(parent)
{
	ui.setupUi(this);

	if (!QSqlDatabase::drivers().contains("QSQLITE"))
		QMessageBox::critical(this, "Unable to load database", "This program needs the SQLITE driver");

	QSqlError err = qtdbop::initDb();
	if (err.type() != QSqlError::NoError) {
		showError(err);
	}

	TableModel* model = new TableModel(ui.mainTableView);

	model->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model->setTable("image");
	model->setHeaderData(model->fieldIndex("iconmat"), Qt::Horizontal, tr("Thumbnail"), Qt::DecorationRole);
	model->setHeaderData(model->fieldIndex("name"),	Qt::Horizontal, tr("Name"));
	model->setHeaderData(simValIdx, Qt::Horizontal, tr("Similarity"));
	model->setHeaderData(model->fieldIndex("dir"), Qt::Horizontal, tr("Directory"));
	ui.mainTableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	ui.mainTableView->verticalHeader()->setDefaultSectionSize(100);
	if (!model->select()) {
		showError(model->lastError());
	}

	int rows = model->rowCount();
	for (int i = 0; i < rows; i++) {
		QModelIndex index = model->index(i, model->fieldIndex("iconmat"));
		model->data(index, Qt::DecorationRole);
		model->setData(index, "125", Qt::EditRole);
	}



	if (!model->select()) {
		showError(model->lastError());
	}
	ui.mainTableView->setModel(model);
	ui.mainTableView->setColumnHidden(model->fieldIndex("id"), true);
	

	ui.imageLabel->setBackgroundRole(QPalette::Base);
	ui.imageLabel->setScaledContents(true);

	resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);

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
	MWObject.connect(ui.openButton, SIGNAL(clicked()), this, SLOT(openList()));
	MWObject.connect(ui.consoleButton, SIGNAL(clicked()), this, SLOT(hideConsole()));
	MWObject.connect(ui.imageLabelButton_2, SIGNAL(clicked()), this, SLOT(openImageLabel()));
}

void MainWindow::showError(const QSqlError& err) {
	QMessageBox::critical(this, "Unable to initialize Database",
		"Error initializing database: " + err.text());
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

	setImage(ui.imageLabel_4, newImage);
	//imageList[0].setImageDirectory(fileName.toStdString());
	setWindowFilePath(fileName);

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