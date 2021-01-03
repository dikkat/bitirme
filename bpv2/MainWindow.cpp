#include "MainWindow.h"

QSqlDatabase qDB = QSqlDatabase();

constexpr int maximum = 16777215;

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

TableModel::TableModel(QObject* parent)	: QSqlQueryModel(parent) {}

QVariant TableModel::data(const QModelIndex& index, int role) const {
	int col = index.column();
	if (col == 0) {
		if (role == Qt::DecorationRole) {
			QString imcode = QSqlQueryModel::data(index).toString();
			string condition = "imhash='" + imcode.toStdString() + "'";
			img::Icon imgIcon(dbop::deserializeMat(imcode.toStdString()));
			int height = imgIcon.getIconMat().rows;
			auto mat = imgIcon.getIconMat();
			std::vector<uchar> buffer;
			cv::imencode(".jpg", mat, buffer, { cv::IMWRITE_JPEG_QUALITY, 85 });
			auto compressed = cv::imdecode(buffer, cv::IMREAD_COLOR);
			auto icon = QIcon(QPixmap::fromImage(cvMatToQImage(compressed)));
			return icon;
		}
		else {
			return QString("N/A");
		}
	}
	
	return QSqlQueryModel::data(index, role);
}

SortDialog::SortDialog(QWidget* parent) {
	ui.setupUi(this);
	current = new float(0);
	enableVec = std::vector<bool>(3, false);
	bgrEnableVec = std::vector<std::vector<bool>>(3, std::vector<bool>(3, true));
	binVec = std::vector<vecf>(3, vecf(3, 0));
	weightVec = std::vector<vecf>(3, vecf(3, 0));
	ui.checkBox_B->setVisible(false);
	ui.checkBox_G->setVisible(false);
	ui.checkBox_R->setVisible(false);
	initlineedit_arr = {ui.lineEdit_18, ui.lineEdit_19, ui.lineEdit_gradirbin, 
		ui.lineEdit_gramagbin, ui.lineEdit_weightgradir, ui.lineEdit_weightgramag, ui.lineEdit_weighthistf,
		ui.lineEdit_weighthists, ui.lineEdit_weighthistt};
	lineedit_hist_arr = { ui.lineEdit_weighthistf, ui.lineEdit_weighthists, ui.lineEdit_weighthistt };
	QDoubleValidator* validator = new QDoubleValidator(0.0001, 100.0, 1000);
	ui.lineEdit_2->setValidator(validator);
	lineedit_arr = initlineedit_arr;
	for (int i = 0; i < lineedit_arr.size(); i++)
		lineedit_arr[i]->setValidator(validator);

	//KEEP BGR AND HSV TICKS SEPARATE
	//BGR AND HSV SHOULD DISABLE BINS
	//WEIGHT SHOULD SET TO VALUE AT NEW INDEX WHEN COMBOBOX CHANGE INDEX
	//BINS SHOULD TOO

	lineedit_arr.erase(lineedit_arr.begin() + 2, lineedit_arr.begin() + 4);

	QObject MWObject;
	parent_ptr = (MainWindow*)parent;
	MWObject.connect(ui.buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(buildFeatureVector()));
	MWObject.connect(ui.buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(buildFeatureVector()));
	MWObject.connect(ui.checkBox_B, &QCheckBox::toggled, [&](bool enable){enableBGR(enable, 0); });
	MWObject.connect(ui.checkBox_G, &QCheckBox::toggled, [&](bool enable) {enableBGR(enable, 1); });
	MWObject.connect(ui.checkBox_R, &QCheckBox::toggled, [&](bool enable) {enableBGR(enable, 2); });
	MWObject.connect(ui.checkBox_sortHash, SIGNAL(toggled(bool)), this, SLOT(enableHash(bool)));
	MWObject.connect(ui.checkBox_sorthistg, SIGNAL(toggled(bool)), this, SLOT(enableHistG(bool)));
	MWObject.connect(ui.checkBox_sorthistb, SIGNAL(toggled(bool)), this, SLOT(enableHistB(bool)));
	MWObject.connect(ui.checkBox_sorthisth, SIGNAL(toggled(bool)), this, SLOT(enableHistH(bool)));
	MWObject.connect(ui.checkBox_sortEdge, SIGNAL(toggled(bool)), this, SLOT(enableEdge(bool)));
	MWObject.connect(ui.checkBox, SIGNAL(toggled(bool)), this, SLOT(equaliseWeights(bool)));
	MWObject.connect(ui.comboBox_histFlag, SIGNAL(currentIndexChanged(int)), this, SLOT(displayButtons_BGR(int)));
	for (int i = 0; i < lineedit_arr.size(); i++) {
		MWObject.connect(lineedit_arr[i], SIGNAL(textChanged(QString)), this, SLOT(remainingPercentage(QString)));
	}
	MWObject.connect(ui.horizontalSlider_fbin, &QSlider::sliderMoved,
		[&](int value) {
			QToolTip::showText(QCursor::pos(), QString("%1").arg(value), nullptr);
			binVec[ui.comboBox_histFlag->currentIndex()][0] = value;
		});
	MWObject.connect(ui.horizontalSlider_sbin, &QSlider::sliderMoved,
		[&](int value) {
			QToolTip::showText(QCursor::pos(), QString("%1").arg(value), nullptr);
			binVec[ui.comboBox_histFlag->currentIndex()][1] = value;
		});
	MWObject.connect(ui.horizontalSlider_tbin, &QSlider::sliderMoved,
		[&](int value) {
			QToolTip::showText(QCursor::pos(), QString("%1").arg(value), nullptr);
			binVec[ui.comboBox_histFlag->currentIndex()][2] = value;
		});
	lineedit_arr.erase(lineedit_arr.end() - 3, lineedit_arr.end());
}

void SortDialog::equaliseWeights(bool enable) {
	iop::WeightVector wv(true);
	auto ftoq = [](float f) {
		return QString::fromStdString(gen::format(f));
	};
	ui.lineEdit_18->setText(ftoq(wv.wv_hash[0] * 100));
	ui.lineEdit_19->setText(ftoq(wv.wv_hash[1] * 100));
	ui.lineEdit_weightgramag->setText(ftoq(wv.wv_grad[0] * 100));
	ui.lineEdit_weightgradir->setText(ftoq(wv.wv_grad[1] * 100));
	weightVec[0][0] = wv.w_hgray * 100;
	weightVec[1][0] = wv.wv_hbgr[0] * 100;
	weightVec[1][1] = wv.wv_hbgr[1] * 100;
	weightVec[1][2] = wv.wv_hbgr[2] * 100;
	weightVec[2][0] = wv.wv_hhsv[0] * 100;
	weightVec[2][1] = wv.wv_hhsv[1] * 100;
	weightVec[2][2] = wv.wv_hhsv[2] * 100;
	binVec[0][0] = 10;
	binVec[1][0] = 5;
	binVec[1][1] = 5;
	binVec[1][2] = 5;
	binVec[2][0] = 5;
	binVec[2][1] = 5;
	binVec[2][2] = 5;
	for (auto& i : bgrEnableVec)
		for (auto& j : i)
			j = true;
	ui.comboBox_edgeFlag->setCurrentIndex(0);
	ui.lineEdit_gradirbin->setText(ftoq(8));
	ui.lineEdit_gramagbin->setText(ftoq(10));
	enableVec[0] = true;
	ui.checkBox_sorthistg->setChecked(true);
	ui.checkBox_sorthistb->setChecked(true);
	ui.checkBox_sorthisth->setChecked(true);
	enableVec[1] = true;
	enableVec[2] = true;
	ui.checkBox_sortEdge->setChecked(true);
	ui.checkBox_sortHash->setChecked(true);
	emit displayButtons_BGR(ui.comboBox_histFlag->currentIndex());
	emit remainingPercentage("", true);
}

void SortDialog::enableBGR(bool enable, int index) {
	QCheckBox* checkbox = (QCheckBox*)sender();
	if (index == 0) {
		ui.horizontalSlider_fbin->setEnabled(enable);
		ui.lineEdit_weighthistf->setEnabled(enable);
		if (ui.comboBox_histFlag->currentIndex() == 1) {
			binVec[1][0] = enable ? ui.horizontalSlider_fbin->value() : 0;
			bgrEnableVec[1][0] = enable;
		}
		else if (ui.comboBox_histFlag->currentIndex() == 2) {
			binVec[2][0] = enable ? ui.horizontalSlider_fbin->value() : 0;
			bgrEnableVec[2][0] = enable;
		}
	}
	else if (index == 1) {
		ui.horizontalSlider_sbin->setEnabled(enable);
		ui.lineEdit_weighthists->setEnabled(enable);
		if (ui.comboBox_histFlag->currentIndex() == 1) {
			binVec[1][1] = enable ? ui.horizontalSlider_sbin->value() : 0;
			bgrEnableVec[1][1] = enable;
		}
		else if (ui.comboBox_histFlag->currentIndex() == 2) {
			binVec[2][1] = enable ? ui.horizontalSlider_sbin->value() : 0;
			bgrEnableVec[2][1] = enable;
		}
	}
	else if (index == 2) {
		ui.horizontalSlider_tbin->setEnabled(enable);
		ui.lineEdit_weighthistt->setEnabled(enable);
		if (ui.comboBox_histFlag->currentIndex() == 1) {
			binVec[1][2] = enable ? ui.horizontalSlider_tbin->value() : 0;
			bgrEnableVec[1][2] = enable;
		}
		else if (ui.comboBox_histFlag->currentIndex() == 2) {
			binVec[2][2] = enable ? ui.horizontalSlider_tbin->value() : 0;
			bgrEnableVec[2][2] = enable;
		}
	}
	emit remainingPercentage("", true);
}

void SortDialog::remainingPercentage(QString sent, bool disabled) {
	float initial = ui.lineEdit_2->text().toFloat();
	QLineEdit* lineedit = (QLineEdit*)sender();
	int iter = -1;
	if(!disabled)
		for (int i = 0; i < lineedit_hist_arr.size(); i++) {
			if (lineedit_hist_arr[i]->objectName() == lineedit->objectName()) {
				iter = i;
				break;
			}
		}
	if (iter != -1) {
		int index = ui.comboBox_histFlag->currentIndex();
		weightVec[index][iter] = sent.toFloat();
	}
	float currentPerc = 0;
	for (auto i : lineedit_arr) {
		if (i->isEnabled())
			currentPerc += i->text().toFloat();
	}
	for (int i = 0; i < weightVec.size(); i++)
		for (int j = 0; j < weightVec[i].size(); j++)
			currentPerc += enableVec[i] && bgrEnableVec[i][j] ? weightVec[i][j] : 0;
	ui.lineEdit_2->setText(QString::fromStdString(std::to_string(100 - currentPerc)));
	if (!gen::cmpf(currentPerc, 100)) {
		ui.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
		ui.buttonBox->button(QDialogButtonBox::Ok)->setToolTip("Weight percentage must be 100.");
	}
	else {
		ui.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
	}
	if (!gen::cmpf(100 - currentPerc, initial))
		ui.checkBox->setChecked(false);
}

void SortDialog::enableHash(bool enable) {
	ui.lineEdit_18->setEnabled(enable);
	ui.lineEdit_19->setEnabled(enable);
	emit remainingPercentage("h", true);
}

void SortDialog::enableHistG(bool enable) {
	ui.checkBox_B->setVisible(false);
	ui.checkBox_G->setVisible(false);
	ui.checkBox_R->setVisible(false);
	ui.horizontalSlider_fbin->setEnabled(enable && bgrEnableVec[0][0]);
	ui.horizontalSlider_sbin->setEnabled(false);
	ui.horizontalSlider_tbin->setEnabled(false);
	ui.horizontalSlider_fbin->setValue(binVec[0][0]);
	ui.horizontalSlider_sbin->setValue(binVec[0][1]);
	ui.horizontalSlider_tbin->setValue(binVec[0][2]);
	ui.lineEdit_weighthistf->setEnabled(enable && bgrEnableVec[0][0]);
	ui.lineEdit_weighthists->setEnabled(false);
	ui.lineEdit_weighthistt->setEnabled(false);
	ui.lineEdit_weighthistf->setText(QString::fromStdString(gen::format(weightVec[0][0])));
	ui.lineEdit_weighthists->setText(QString::fromStdString(gen::format(weightVec[0][1])));
	ui.lineEdit_weighthistt->setText(QString::fromStdString(gen::format(weightVec[0][2])));
	ui.horizontalSlider_fbin->setMaximum(255);
	ui.label_bMax->setText("255");
	enableVec[0] = enable;
	emit remainingPercentage("g", true);
}

void SortDialog::enableHistB(bool enable) {
	ui.checkBox_B->setText("B");
	ui.checkBox_B->setVisible(true);
	ui.checkBox_B->setEnabled(enable);
	ui.checkBox_G->setText("G");
	ui.checkBox_G->setVisible(true);
	ui.checkBox_G->setEnabled(enable);
	ui.checkBox_R->setText("R");
	ui.checkBox_R->setVisible(true);
	ui.checkBox_R->setEnabled(enable);
	ui.horizontalSlider_fbin->setEnabled(enable && bgrEnableVec[1][0]);
	ui.horizontalSlider_sbin->setEnabled(enable && bgrEnableVec[1][1]);
	ui.horizontalSlider_tbin->setEnabled(enable && bgrEnableVec[1][2]);
	ui.horizontalSlider_fbin->setMaximum(255);
	ui.label_bMax->setText("255");
	ui.horizontalSlider_fbin->setValue(binVec[1][0]);
	ui.horizontalSlider_sbin->setValue(binVec[1][1]);
	ui.horizontalSlider_tbin->setValue(binVec[1][2]);
	ui.lineEdit_weighthistf->setEnabled(enable && bgrEnableVec[1][0]);
	ui.lineEdit_weighthists->setEnabled(enable && bgrEnableVec[1][1]);
	ui.lineEdit_weighthistt->setEnabled(enable && bgrEnableVec[1][2]);
	ui.lineEdit_weighthistf->setText(QString::fromStdString(gen::format(weightVec[1][0])));
	ui.lineEdit_weighthists->setText(QString::fromStdString(gen::format(weightVec[1][1])));
	ui.lineEdit_weighthistt->setText(QString::fromStdString(gen::format(weightVec[1][2])));
	enableVec[1] = enable;
	emit remainingPercentage("b", true);
}

void SortDialog::enableHistH(bool enable) {
	ui.checkBox_B->setText("H");
	ui.checkBox_B->setVisible(true);
	ui.checkBox_B->setEnabled(enable);
	ui.checkBox_G->setText("S");
	ui.checkBox_G->setVisible(true);
	ui.checkBox_G->setEnabled(enable);
	ui.checkBox_R->setText("V");
	ui.checkBox_R->setVisible(true);
	ui.checkBox_R->setEnabled(enable);
	ui.horizontalSlider_fbin->setEnabled(enable && bgrEnableVec[2][0]);
	ui.horizontalSlider_sbin->setEnabled(enable && bgrEnableVec[2][1]);
	ui.horizontalSlider_tbin->setEnabled(enable && bgrEnableVec[2][2]);
	ui.horizontalSlider_fbin->setMaximum(180);
	ui.label_bMax->setText("180");
	ui.horizontalSlider_fbin->setValue(binVec[2][0]);
	ui.horizontalSlider_sbin->setValue(binVec[2][1]);
	ui.horizontalSlider_tbin->setValue(binVec[2][2]);
	ui.lineEdit_weighthistf->setEnabled(enable && bgrEnableVec[2][0]);
	ui.lineEdit_weighthists->setEnabled(enable && bgrEnableVec[2][1]);
	ui.lineEdit_weighthistt->setEnabled(enable && bgrEnableVec[2][2]);
	ui.lineEdit_weighthistf->setText(QString::fromStdString(gen::format(weightVec[2][0])));
	ui.lineEdit_weighthists->setText(QString::fromStdString(gen::format(weightVec[2][1])));
	ui.lineEdit_weighthistt->setText(QString::fromStdString(gen::format(weightVec[2][2])));
	enableVec[2] = enable;
	emit remainingPercentage("s", true);
}

void SortDialog::enableEdge(bool enable) {
	ui.comboBox_edgeFlag->setEnabled(enable);
	ui.lineEdit_weightgramag->setEnabled(enable);
	ui.lineEdit_weightgradir->setEnabled(enable);
	ui.lineEdit_gramagbin->setEnabled(enable);
	ui.lineEdit_gradirbin->setEnabled(enable);
	emit remainingPercentage("e", true);
}

void SortDialog::buildFeatureVector() {
	iop::WeightVector wv(true);
	wv.wv_hash[0] = ui.lineEdit_18->text().toFloat() / 100;
	wv.wv_hash[1] = ui.lineEdit_19->text().toFloat() / 100;
	wv.wv_grad[0] = ui.lineEdit_weightgramag->text().toFloat() / 100;
	wv.wv_grad[1] = ui.lineEdit_weightgradir->text().toFloat() / 100;
	wv.w_hgray = weightVec[0][0] / 100;
	wv.wv_hbgr[0] = weightVec[1][0] / 100;
	wv.wv_hbgr[1] = weightVec[1][1] / 100;
	wv.wv_hbgr[2] = weightVec[1][2] / 100;
	wv.wv_hhsv[0] = weightVec[2][0] / 100;
	wv.wv_hhsv[1] = weightVec[2][1] / 100;
	wv.wv_hhsv[2] = weightVec[2][2] / 100;

	feat::Histogram hist_g;
	if(enableVec[0])
		hist_g = feat::Histogram(cv::Mat(), HIST_GRAY, binVec[0][0]);
	else
		hist_g = feat::Histogram(cv::Mat(), HIST_GRAY, 0);

	feat::Histogram hist_bgr;
	if (enableVec[1])
		hist_bgr = feat::Histogram(cv::Mat(), HIST_BGR, bgrEnableVec[1][0] ? binVec[1][0] : 0, 
			bgrEnableVec[1][1] ? binVec[1][1] : 0, 
			bgrEnableVec[1][2] ? binVec[1][2] : 0);
	else
		hist_bgr = feat::Histogram(cv::Mat(), HIST_BGR, 0, 0, 0);

	feat::Histogram hist_hsv;
	if (enableVec[2])
		hist_hsv = feat::Histogram(cv::Mat(), HIST_HSV, bgrEnableVec[2][0] ? binVec[2][0] : 0,
			bgrEnableVec[2][1] ? binVec[2][1] : 0,
			bgrEnableVec[2][2] ? binVec[2][2] : 0);
	else
		hist_hsv = feat::Histogram(cv::Mat(), HIST_HSV, 0, 0, 0);

	feat::Edge edge;
	if (ui.checkBox_sortEdge)
		edge = feat::Edge(cv::Mat(), ui.comboBox_edgeFlag->currentIndex(), nullptr,
			-1, ui.lineEdit_gramagbin->text().toFloat(), ui.lineEdit_gradirbin->text().toFloat());
	else
		edge = feat::Edge(cv::Mat(), 0, nullptr, -1, 0, 0);
	
	feat::Hash hash;
	if (ui.checkBox_sortHash)
		hash = feat::Hash(cv::Mat(), std::make_pair(true, true));

	iop::FeatureVector fv(nullptr, &edge, &hist_g, &hist_bgr, &hist_hsv, &hash);

	emit parent_ptr->setFeatureVector(fv, wv);
}

void SortDialog::displayButtons_BGR(int state) {
	if (state == 0) {
		enableHistG(enableVec[0]);
		ui.stackedWidget->setCurrentIndex(0);
	}
	else if (state == 1) {
		ui.stackedWidget->setCurrentIndex(1);
		ui.checkBox_B->setChecked(bgrEnableVec[1][0]);
		ui.checkBox_G->setChecked(bgrEnableVec[1][1]);
		ui.checkBox_R->setChecked(bgrEnableVec[1][2]);
		enableHistB(enableVec[1]);
	}
	else if (state == 2) {
		ui.stackedWidget->setCurrentIndex(2);
		ui.checkBox_B->setChecked(bgrEnableVec[2][0]);
		ui.checkBox_G->setChecked(bgrEnableVec[2][1]);
		ui.checkBox_R->setChecked(bgrEnableVec[2][2]);
		enableHistH(enableVec[2]);
	}
	if (binstate != nullptr)
		delete(binstate);
	binstate = new int(state);
}

MainWindow::MainWindow(dbop::Database dbObj, QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);

	setIcons();
	screenSize = new QSize(QApplication::desktop()->screenGeometry().width(), QApplication::desktop()->screenGeometry().height());

	QResource::registerResource("Resource.rcc");

	mw_dbPtr = &dbObj;

	qDB = QSqlDatabase::addDatabase("QSQLITE");
	qDB.setDatabaseName("C:/Users/ASUS/source/repos/bpv2/bpv2/bitirme.db");

	if (!qDB.open())
		showError(qDB.lastError());

	resultModel = new TableModel;
	resultModel->setQuery("SELECT DISTINCT icon.mat, b.name, similarity.similarity"
		" FROM icon, image a, image b"
		" INNER JOIN imageicon"
		" ON imageicon.imhash = b.hash and imageicon.iconhash = icon.hash"
		" INNER JOIN similarity"
		" ON similarity.srchash = a.hash and similarity.desthash = b.hash", qDB);
	ui.resultTableView->setModel(resultModel);
	resultModel->setHeaderData(0, Qt::Horizontal, tr("Thumbnail"), Qt::DecorationRole);
	resultModel->setHeaderData(1, Qt::Horizontal, tr("Name"));
	resultModel->setHeaderData(2, Qt::Horizontal, tr("Similarity"));
	
	ui.resultTableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	ui.resultTableView->verticalHeader()->setDefaultSectionSize(100);
	ui.resultTableView->verticalHeader()->setVisible(false);

	ui.resultTableView->horizontalHeader()->setModel(resultModel);
	ui.resultTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
	ui.resultTableView->verticalHeader()->setDefaultSectionSize(100);
	ui.resultTableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
	ui.resultTableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);

	mainModel = new TableModel;
	mainModel->setQuery("SELECT DISTINCT icon.mat, a.name, a.hash"
		" FROM icon, image a"
		" INNER JOIN imageicon"
		" ON imageicon.imhash = a.hash and imageicon.iconhash = icon.hash", qDB);
	ui.mainTableView->setModel(mainModel);
	mainModel->setHeaderData(0, Qt::Horizontal, tr("Thumbnail"), Qt::DecorationRole);
	mainModel->setHeaderData(1, Qt::Horizontal, tr("Name"));
	mainModel->setHeaderData(2, Qt::Horizontal, tr("Hash"));
	
	ui.mainTableView->setColumnHidden(2, true);

	ui.mainTableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	ui.mainTableView->verticalHeader()->setDefaultSectionSize(100);
	ui.mainTableView->verticalHeader()->setVisible(false);

	ui.mainTableView->horizontalHeader()->setModel(mainModel);
	ui.mainTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
	ui.mainTableView->verticalHeader()->setDefaultSectionSize(100);
	ui.mainTableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

	ui.mainTableView->setContextMenuPolicy(Qt::CustomContextMenu);

	ui.label_imgSrc->setBackgroundRole(QPalette::Base);
	ui.label_imgSrc->setScaledContents(true);

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

	ui.comboBox_kernelFlag->setEnabled(false);
	ui.lineEdit_gauss->setEnabled(false);
	ui.lineEdit_tlow->setEnabled(false);
	ui.lineEdit_thigh->setEnabled(false);
	ui.lineEdit_sigma->setEnabled(false);

	QObject MWObject;
	MWObject.connect(ui.pushButton_srcImgLabel, SIGNAL(clicked()), this, SLOT(openImageLabel()));
	MWObject.connect(ui.comboBox_edgeFlag, SIGNAL(currentIndexChanged(int)), this, SLOT(enableCanny(int)));
	MWObject.connect(ui.pushButton_sort, SIGNAL(clicked()), this, SLOT(compareImages()));
	MWObject.connect(ui.pushButton_loadImgs, SIGNAL(clicked()), this, SLOT(openList()));
	MWObject.connect(ui.pushButton_dispInDet, SIGNAL(clicked()), this, SLOT(displayFeature()));
	MWObject.connect(ui.comboBox_histFlag, SIGNAL(currentIndexChanged(int)), this, SLOT(displayButtons_BGR()));
	MWObject.connect(ui.mainTableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customMenuRequested(QPoint)));
	MWObject.connect(ui.toolButton_settings, SIGNAL(clicked()), this, SLOT(openSortDialog()));
	MWObject.connect(ui.checkBox_icon, SIGNAL(toggled(bool)), this, SLOT(showIcons(bool)));
	MWObject.connect(ui.horizontalSlider_fbin, &QSlider::sliderMoved,
		[&](int value) {
#if 0 // not so nice -> delayed
			qSlider.setToolTip(QString("%1").arg(value));
#else // better
			QToolTip::showText(QCursor::pos(), QString("%1").arg(value), nullptr);
#endif // 0
		});
	MWObject.connect(ui.horizontalSlider_sbin, &QSlider::sliderMoved,
		[&](int value) {
#if 0 // not so nice -> delayed
			qSlider.setToolTip(QString("%1").arg(value));
#else // better
			QToolTip::showText(QCursor::pos(), QString("%1").arg(value), nullptr);
#endif // 0
		});
	MWObject.connect(ui.horizontalSlider_sbin, &QSlider::sliderMoved,
		[&](int value) {
#if 0 // not so nice -> delayed
			qSlider.setToolTip(QString("%1").arg(value));
#else // better
			QToolTip::showText(QCursor::pos(), QString("%1").arg(value), nullptr);
#endif // 0
		});
	
}

MainWindow::~MainWindow(){
	mw_dbPtr->delete_GENERAL(std::vector<string>{"sourceimage"});
}

void MainWindow::setIcons() {
	ui.toolButton_settings->setIcon(QIcon(":/new/prefix1/Resources/qt resources/settings.png"));
	ui.pushButton_switch->setIcon(QIcon(":/new/prefix1/Resources/qt resources/swap.png"));
	ui.pushButton_sort->setIcon(QIcon(":/new/prefix1/Resources/qt resources/sort.png"));
	ui.pushButton_6->setIcon(QIcon(":/new/prefix1/Resources/qt resources/magnifying-glass.png"));
	ui.pushButton_10->setIcon(QIcon(":/new/prefix1/Resources/qt resources/magnifying-glass.png"));
	ui.pushButton_14->setIcon(QIcon(":/new/prefix1/Resources/qt resources/magnifying-glass.png"));
	ui.pushButton_18->setIcon(QIcon(":/new/prefix1/Resources/qt resources/magnifying-glass.png"));
	ui.pushButton_5->setIcon(QIcon(":/new/prefix1/Resources/qt resources/magnifying-glass(1).png"));
	ui.pushButton_9->setIcon(QIcon(":/new/prefix1/Resources/qt resources/magnifying-glass(1).png"));
	ui.pushButton_13->setIcon(QIcon(":/new/prefix1/Resources/qt resources/magnifying-glass(1).png"));
	ui.pushButton_17->setIcon(QIcon(":/new/prefix1/Resources/qt resources/magnifying-glass(1).png"));
	ui.pushButton_dispInDet->setIcon(QIcon(":/new/prefix1/Resources/qt resources/compare.png"));
	ui.pushButton_dispInDet_2->setIcon(QIcon(":/new/prefix1/Resources/qt resources/compare.png"));
}

void MainWindow::openSortDialog() {
	SortDialog sortDialog(this);
	sortDialog.exec();
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

void MainWindow::setFeatureVector(iop::FeatureVector returnfv, iop::WeightVector returnwv) {
	if (currentfv)
		delete(currentfv);
	currentfv = new iop::FeatureVector(returnfv);

	if (currentwv)
		delete(currentwv);
	currentwv = new iop::WeightVector(returnwv);
}

void MainWindow::showIcons(bool show) {
	if (show) {
		ui.mainTableView->setColumnHidden(0, false);
		ui.resultTableView->setColumnHidden(0, false);
	}
	else {
		ui.mainTableView->setColumnHidden(0, true);
		ui.resultTableView->setColumnHidden(0, true);
	}
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

void MainWindow::scaleImage(QImage& image, QLabel* imlabel, QWidget* frame) {
	float width, height;
	if (image.width() >= image.height()) {
		width = frame->width() > screenSize->width() * 6 / 20 ? screenSize->width() * 6 / 20 : frame->width();
		height = width * image.height() / image.width();
		imlabel->setMaximumHeight(height);
		imlabel->setMaximumWidth(maximum);
	}
	else if (image.height() >= image.width()) {
		height = frame->height() > screenSize->height() * 6 / 20 ? screenSize->height() * 6 / 20 : frame->height();
		width = height * image.height() / image.width();
		imlabel->setMaximumWidth(width);
		imlabel->setMaximumHeight(maximum);
	}
	image = image.scaled(width, height, Qt::KeepAspectRatio);
}

void MainWindow::displayFeature() {
	int index = ui.tabWidget_comparison->currentIndex();
	img::Image srcImg = mw_dbPtr->select_SourceImage();
	img::Image destImg = mw_dbPtr->select_DestinationImage();

	QImage srcImgScaled = cvMatToQImage(srcImg.getImageMat());
	QImage destImgScaled = cvMatToQImage(destImg.getImageMat());

	scaleImage(srcImgScaled, ui.label_imgSrcBig, ui.frame_src);
	scaleImage(destImgScaled, ui.label_imgDestBig, ui.frame_dest);

	if (index == 0) {
		displayHash(&srcImg);
	}
	else if (index == 1) {
		ui.label_imgSrcBig->setPixmap(QPixmap::fromImage(srcImgScaled));
		ui.label_imgDestBig->setPixmap(QPixmap::fromImage(destImgScaled));
		displayHistogram(&srcImg, true);
		displayHistogram(&destImg, false);
		ui.mainTabWidget->setCurrentIndex(1);
	}
	else if (index == 2) {
		ui.label_imgSrcBig->setPixmap(QPixmap::fromImage(srcImgScaled));
		ui.label_imgDestBig->setPixmap(QPixmap::fromImage(destImgScaled));
		displayEdge(&srcImg, true);
		displayEdge(&destImg, false);
		ui.mainTabWidget->setCurrentIndex(1);
	}
	else if (index == 3) {
		ui.label_imgSrcBig->setPixmap(QPixmap::fromImage(srcImgScaled));
		ui.label_imgDestBig->setPixmap(QPixmap::fromImage(destImgScaled));
		displayCorner(&srcImg, true);
		displayCorner(&destImg, false);
		ui.mainTabWidget->setCurrentIndex(1);
	}
}

void MainWindow::displayHash(img::Image* src) {
	if (ui.label_imgSrc->pixmap()->isNull()) {
		throw std::exception("Unable to locate source image. Load image to source first.");
	}
	else {
		/*string srcDHash = feat::Hash::imageHashing_dHash(QImageToCvMat(ui.label_imgSrc->pixmap()->toImage())).to_string();
		ui.label_imHash->setText(srcDHash.c_str());*/ //TODO: COM BACK 'ER
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

	switchDisplayWidgets(false, source);

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
		feat::Edge::Canny* canny = new feat::Edge::Canny(edgeVals[1], edgeVals[4], edgeVals[3], edgeVals[2], kernelX, kernelY);
		srcEdge = lnkr::setEdge(src, edgeVals[0], canny);
		delete(canny);
	}

	else {
		srcEdge = lnkr::setEdge(src, edgeVals[0]);
	}
	QImage image = cvMatToQImage(srcEdge->getEdgeMat());
	if (source) {
		QLabel* imlabel = ui.label_derSrcBig;
		scaleImage(image, imlabel, ui.stackedWidget_src);
		ui.label_derSrcBig->setPixmap(QPixmap::fromImage(image));
	}
	else {
		QLabel* imlabel = ui.label_derDestBig;
		scaleImage(image, imlabel, ui.stackedWidget_dest);
		ui.label_derDestBig->setPixmap(QPixmap::fromImage(image));
	}

	switchDisplayWidgets(true, source);
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

bool MainWindow::loadFile(const QString& fileName, bool source)
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
	setImage(source ? ui.label_imgSrc : ui.label_imgDest, newImage);

	setWindowFilePath(fileName);
	source ? lnkr::setSourceImage(QDir::cleanPath(fileName).toStdString(), cv::IMREAD_COLOR) 
		: lnkr::setDestinationImage(QDir::cleanPath(fileName).toStdString(), cv::IMREAD_COLOR);

	return true;
}

void MainWindow::setImage(QLabel* imlabel, const QImage& newImage) {
	QImage image = newImage;
	if (image.colorSpace().isValid())
		image.convertToColorSpace(QColorSpace::SRgb);

	float width, height;
	scaleImage(image, imlabel, ui.frame);
	imlabel->setPixmap(QPixmap::fromImage(image));
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
	dialog.setOption(QFileDialog::DontUseNativeDialog, true);

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

void MainWindow::addToMainTable(img::Image* image) {
	lnkr::addToMainTable(image);
	mainModel->query().exec();
	mainModel->setQuery(mainModel->query());
}



void MainWindow::createActions()
//! [17] //! [18]
{
	
}

void MainWindow::customMenuRequested(QPoint pos) {
	QModelIndex index = ui.mainTableView->indexAt(pos);
	auto hash = mainModel->data(index.siblingAtColumn(2));
	QSqlQuery query("SELECT dir FROM image WHERE hash='" + hash.toString() + "'", qDB);
	query.exec();
	while (query.next()) {
		if(dir) delete(dir);
		dir = new QString(query.value(0).toString());
	}

	QMenu* menu = new QMenu(this);
	QAction* action1 = new QAction("Pick as source image", menu);
	QAction* action2 = new QAction("Pick as comparison image", menu);
	QAction* action3 = new QAction("Delete from table(will also delete from database)", menu);
	QAction* action4 = new QAction("Copy full path to clipboard", menu);
	QAction* action5 = new QAction("Copy folder path to clipboard", menu);

	menu->addAction(action1);
	menu->addAction(action2);
	menu->addAction(action3);
	menu->addAction(action4);
	menu->popup(ui.mainTableView->viewport()->mapToGlobal(pos));

	QObject::connect(action1, &QAction::triggered, this, [this] { loadFile(*dir); });
	QObject::connect(action2, &QAction::triggered, this, [this] { loadFile(*dir, false); });
	QObject::connect(action3, &QAction::triggered, this, [this] { deleteImage(*dir); });
	QObject::connect(action4, &QAction::triggered, this, [this] { copyToClipboard(*dir); });
	QObject::connect(action5, &QAction::triggered, this, [this] { copyToClipboard(*dir, true); });
}

void MainWindow::copyToClipboard(QString& str, bool folder) {
	QClipboard* clipboard = QApplication::clipboard();

	if (folder) {
		QString folderPath;
		int j = 0;
		for (int i = 0; i < dir->size(); i++) {
			if (dir[i] == '/' || dir[i] == '\\') {
				j = i;
			}
		}
		folderPath = str.toStdString().substr(0, j + 1).c_str();
		clipboard->setText(folderPath);
		statusBar()->showMessage("Directory " + folderPath + " copied to clipboard.", 5000);
	}
	else {
		clipboard->setText(str);
		statusBar()->showMessage("Directory " + str + " copied to clipboard.", 5000);
	}
}

void MainWindow::deleteImage(QString& fileName) {
	auto image = lnkr::createImage(fileName.toStdString(), cv::IMREAD_COLOR);
	lnkr::deleteImage(&image);
	mainModel->query().exec();
	mainModel->setQuery(mainModel->query());
}

void MainWindow::refreshTable(TableModel* table, bool main) {
	QString query = table->query().executedQuery();
	table->clear();
	table->query().clear();
	table->setQuery(query);
	if (main) {
		mainModel->setHeaderData(0, Qt::Horizontal, tr("Thumbnail"), Qt::DecorationRole);
		mainModel->setHeaderData(1, Qt::Horizontal, tr("Name"));
		mainModel->setHeaderData(2, Qt::Horizontal, tr("Hash"));

		ui.mainTableView->setColumnHidden(2, true);

		ui.mainTableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
		ui.mainTableView->verticalHeader()->setDefaultSectionSize(100);
		ui.mainTableView->verticalHeader()->setVisible(false);

		ui.mainTableView->horizontalHeader()->setModel(mainModel);
		ui.mainTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
		ui.mainTableView->verticalHeader()->setDefaultSectionSize(100);
		ui.mainTableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

		ui.mainTableView->setContextMenuPolicy(Qt::CustomContextMenu);
	}
}

void MainWindow::enableCanny(int state) {
	if (state == 3) {
		ui.comboBox_kernelFlag->setEnabled(true);
		ui.lineEdit_gauss->setEnabled(true);
		ui.lineEdit_tlow->setEnabled(true);
		ui.lineEdit_thigh->setEnabled(true);
		ui.lineEdit_sigma->setEnabled(true);
	}
	else {
		ui.comboBox_kernelFlag->setEnabled(false);
		ui.lineEdit_gauss->setEnabled(false);
		ui.lineEdit_tlow->setEnabled(false);
		ui.lineEdit_thigh->setEnabled(false);
		ui.lineEdit_sigma->setEnabled(false);
	}
}