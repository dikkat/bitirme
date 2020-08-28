#include "MainWindow.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>



img::Image QImageToimgImage(const QImage& operand) {
	cv::Mat mat = cv::Mat(operand.height(), operand.width(), CV_8UC4, (uchar*)operand.bits(), operand.bytesPerLine());
	cv::Mat result = cv::Mat(mat.rows, mat.cols, CV_8UC3);
	int from_to[] = { 0,0,  1,1,  2,2 };
	cv::mixChannels(&mat, 1, &result, 1, from_to, 3);
	img::Image imoperator(result);
	return imoperator;
}

QImage imgImageToQImage(img::Image& operand) {
	QImage imgIn = QImage((uchar*)operand.getImageMat().data, operand.getImageMat().cols, operand.getImageMat().rows, operand.getImageMat().step, QImage::Format_BGR888);
	return imgIn;
}

TableModel::TableModel(QObject* parent)
	: QAbstractTableModel(parent)
{
}

int TableModel::rowCount(const QModelIndex& /*parent*/) const
{
	return ROWS;
}

int TableModel::columnCount(const QModelIndex& /*parent*/) const
{
	return COLS;
}

QVariant TableModel::data(const QModelIndex& index, int role) const
{
	int row = index.row();
	int col = index.column();
	
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

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
		switch (section) {
		case 0:
			return QString("first");
		case 1:
			return QString("second");
		case 2:
			return QString("third");
		}
	}
	return QVariant(); 
}

void TableModel::populateData(std::vector<img::Image> data) {
	modelData.clear();
	for (int i = 0; i < data.size(); i++) {
		modelData.push_back(&data[i]);
		std::cout << modelData[i]->getImageMat().data;
	}
	return;
}

MainWindow::MainWindow(QWidget *parent)	: QMainWindow(parent)
{
	ui.setupUi(this);

	

	ui.imageLabel->setBackgroundRole(QPalette::Base);
	ui.imageLabel->setScaledContents(true);

	resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);

	printToScreen("C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/testbridge.txt", true);

	img::Image imb("C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/testImage.jpg", cv::IMREAD_COLOR);
	setImage(ui.imageLabel, imgImageToQImage(imb));
	
	std::vector<img::Image> xd;
	for (int i = 0; i < 6; i++) {
		img::Image ima("C:/Users/ASUS/source/repos/bpv2/bpv2/Resources/ukbench/full/ukbench0000" + std::to_string(i) + ".jpg", cv::IMREAD_COLOR);
		xd.push_back(ima);
		addToList(ui.listWidget, imgImageToQImage(xd[i]));
	}

	
	TableModel* myModel = new TableModel(this);
	myModel->populateData(xd);
	ui.tableView->setModel(myModel);
	ui.tableView->show();
	ui.tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	
	QObject MWObject;
	MWObject.connect(ui.openButton, SIGNAL(clicked()), this, SLOT(openList()));
	MWObject.connect(ui.consoleButton, SIGNAL(clicked()), this, SLOT(hideConsole()));
	MWObject.connect(ui.imageLabelButton_2, SIGNAL(clicked()), this, SLOT(openImageLabel()));
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
	std::vector<QImage> imoperator;
	for (int i = 0; i < fileNames.size(); i++) {
		QImageReader reader(fileNames[i]);
		reader.setAutoTransform(true);
		const QImage newImage = reader.read();
		if (newImage.isNull()) {
			QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
				tr("Cannot load %1: %2")
				.arg(QDir::toNativeSeparators(fileNames[i]), reader.errorString()));
			continue;
		}
		imoperator.push_back(newImage);
	}
	for (auto a : imoperator)
		addToList(ui.listWidget, a);
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

void MainWindow::addToList(QListWidget* list, const QImage& image) { 
	QListWidgetItem* xd = new QListWidgetItem;
	xd->setIcon(QIcon(QPixmap::fromImage(image)));
	list->addItem(xd);
}

void MainWindow::createActions()
//! [17] //! [18]
{
	
}