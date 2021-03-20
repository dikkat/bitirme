#include "MainWindow.h"
#define CMP_MAX 10000

QSqlDatabase qDB = QSqlDatabase();

constexpr int maximum = 16777215;
constexpr int tableIconSize = 100;
constexpr bool imageCheck = false;

cv::Mat QImageToCvMat(const QImage& operand) {
	cv::Mat mat = cv::Mat(operand.height(), operand.width(), CV_8UC4, (uchar*)operand.bits(), operand.bytesPerLine());
	cv::Mat result = cv::Mat(mat.rows, mat.cols, CV_8UC3);
	int from_to[] = { 0,0,  1,1,  2,2 };
	cv::mixChannels(&mat, 1, &result, 1, from_to, 3);
	return result;
}

QImage cvMatToQImage(const cv::Mat& operand) {
	QImage imgIn;
	if (operand.channels() == 3)
		imgIn = QImage((uchar*)operand.data, operand.cols, operand.rows, operand.step, QImage::Format_BGR888);
	else if (operand.channels() == 1) {
		cv::imwrite("buffer.jpg", operand);
		imgIn = QImage("buffer.jpg");
	}
	else
		throw gen::Warning("Number of image channels can't be different than 1 and 3.");
	return imgIn;
}

SortProxyModel::SortProxyModel(QTableView* parent_table, QObject* parent) : QSortFilterProxyModel(parent), 
	parent_table(parent_table) {
	interval = { 0, maximum };
}

QVariant SortProxyModel::data(const QModelIndex& index, int role) const {
	int row = index.row();
	int pos_main = parent_table->indexAt(parent_table->rect().topLeft()).row();
	std::pair<int, int> datainterval = { pos_main - 5, pos_main + 50 };
	if (datainterval.first <= row && datainterval.second >= row) {
		auto index_main = this->mapToSource(index);
		return this->sourceModel()->data(index_main, role);
	}
	return QVariant();
}

void SortProxyModel::suspendData(std::pair<int, int> interval) {
	this->interval = interval;
	std::thread suspend_thr([this] {
		Sleep(700);
		this->interval = { 0, maximum };
		});
	suspend_thr.detach();
}

TableModel::TableModel(table_type type, std::vector<std::pair<img::Image, float>> imgVec, QObject* parent) :
	type(type), imgVec(imgVec), QAbstractTableModel(parent) {
	iconCache = new QCache<int, cv::Mat>(200);
	colCount = type == table_type::TABLE_MAIN ? 3 : 4;
	rowPos = 0;
	iconColumnHidden = true;
}

int TableModel::rowCount(const QModelIndex& parent) const {
	return dispVec.size();
}

int TableModel::columnCount(const QModelIndex& parent) const {
	return colCount;
}

bool TableModel::insertRows(int row, int count, const QModelIndex& parent) {
	beginInsertRows(QModelIndex(), row, row + count - 1);
	imgVec.insert(imgVec.begin() + row, count, { img::Image(), float{ -1 } } );
	endInsertRows();
	return true;
}

bool TableModel::removeRows(int row, int count, const QModelIndex& parent) {
	if (count == 0) {
		return true;
	}
	beginRemoveRows(QModelIndex(), row, row + count - 1);
	imgVec.erase(imgVec.begin() + row, imgVec.begin() + row + count);
	endRemoveRows();
	return true;
}

bool TableModel::setData(const QModelIndex& index, const QVariant& value, int role) {
	if (index.isValid()) {
		if (index.column() == 0) {
			imgVec.at(index.row()).first.dir = value.toString().toStdString();
			emit dataChanged(index, index, { role });
			return true;
		}
		else if (index.column() == 1) {
			auto hash = value.toString().toStdString();
			imgVec.at(index.row()).first.hash = std::stoull(hash, nullptr); //TODO: CHECK THIS
			emit dataChanged(index, index, { role });
			return true;
		}
		else if (index.column() == 2) {
			imgVec.at(index.row()).first.name = value.toString().toStdString();
			emit dataChanged(index, index, { role });
			return true;
		}
		else if (index.column() == 3) {
			imgVec.at(index.row()).second = value.toFloat();
			emit dataChanged(index, index, { role });
			return true;
		}
	}

	return false;
}

bool TableModel::canFetchMore(const QModelIndex& parent) const {
	if (parent.isValid())
		return false;
	return (rowPos < imgVec.size());
}

void TableModel::fetchMore(const QModelIndex& parent) {
	if (parent.isValid())
		return;
	int remainder = imgVec.size() - rowPos;
	int itemsToFetch = /*this->type == table_type::TABLE_RESULT ?
		remainder : qMin(this->iconColumnHidden ? 100 : 20, */remainder/*)*/;

	if (itemsToFetch <= 0)
		return;

	beginInsertRows(QModelIndex(), rowPos, rowPos + itemsToFetch - 1);

	for(int i = rowPos; i < rowPos + itemsToFetch; i++)
		dispVec.push_back({ &imgVec[i].first, &imgVec[i].second });

	rowPos += itemsToFetch;

	endInsertRows();
}

void TableModel::insertImage(const img::Image& image) {
	/*int next_index = this->rowCount();*/

	imgVec.push_back({ image, -1 });
	dispVec.clear();
	for(int i = 0; i < imgVec.size(); i++) 
		dispVec.push_back({ &imgVec[i].first, &imgVec[i].second });
	/*this->insertRows(next_index, 1);
	this->setData(index(next_index, 0), QVariant(image.dir.c_str()));
	this->setData(index(next_index, 1), QVariant(std::to_string(image.hash).c_str()));
	this->setData(index(next_index, 2), QVariant(image.name.c_str()));
	if (this->type == table_type::TABLE_RESULT)
		this->setData(index(next_index, 3), QVariant(-1));*/
}

void TableModel::insertComparison(const iop::Comparison comparison) {
	if (this->type != table_type::TABLE_RESULT)
		throw std::exception("Illegal table type.");

	img::Image image(comparison.rhand_dir, IMG_EMPTY);
	imgVec.push_back({ image, comparison.euc_dist });
	/*int next_index = this->rowCount();

	this->insertRows(next_index, 1);
	this->setData(index(next_index, 0), QVariant(image.dir.c_str()));
	this->setData(index(next_index, 1), QVariant(std::to_string(image.hash).c_str()));
	this->setData(index(next_index, 2), QVariant(image.name.c_str()));
	this->setData(index(next_index, 3), QVariant(comparison.euc_dist));*/
}

QVariant TableModel::data(const QModelIndex& index, int role) const {
	int col = index.column();
	int row = index.row();

	if (!index.isValid())
		return QVariant();

	if (index.row() >= dispVec.size())
		return QVariant();

	if (col == 0) {
		if (role == Qt::ItemDataRole::DecorationRole) {
			if (iconCache->contains(row)) {
				return QIcon(QPixmap(QPixmap::fromImage(cvMatToQImage(*iconCache->object(row)))));
			}
			else {
				string imdir = dispVec.at(row).first->getVariablesString()[1];
				if (imdir == "")
					return QVariant();
				img::Image image(imdir, cv::IMREAD_COLOR);
				cv::Mat& mat = image.getImageMat();
				auto width = mat.cols;
				auto height = mat.rows;
				auto recommended = tableIconSize;
				if (height > width) {
					width = recommended * width / height;
					height = recommended;
				}
				else {
					height = recommended * height / width;
					width = recommended;
				}

				cv::resize(mat, mat, cv::Size(width, height));
				iconCache->insert(index.row(), new cv::Mat(mat), 1);
				QPixmap pixmap = QPixmap::fromImage(cvMatToQImage(mat));
				return QIcon(pixmap);
			}
		}
		else {
			return QVariant();
		}
	}

	else if (col == 1 && role == Qt::ItemDataRole::DisplayRole) {
		return QString(std::to_string(dispVec[row].first->getHash()).c_str());
	}

	else if (col == 2 && role == Qt::ItemDataRole::DisplayRole) {
		return QString(dispVec[row].first->getVariablesString()[0].c_str());
	}

	else if (col == 3 && role == Qt::ItemDataRole::DisplayRole && type == table_type::TABLE_RESULT) {
		return QVariant(*dispVec[row].second);
	}
	
	return QVariant();
}

table_type TableModel::getType() {
	return type;
}

void TableModel::setIconColumnHidden(bool hidden) {
	iconColumnHidden = hidden;
}

void TableModel::updateComparison(int row, const iop::Comparison comparison) {
	if (this->type != table_type::TABLE_RESULT)
		throw std::exception("Comparison can't be added to main table.");
	imgVec.at(row).second = comparison.euc_dist;
	//emit dataChanged(this->index(row, 3), this->index(row, 3));
}

std::vector<std::pair<img::Image, float>>* TableModel::getImgVecPtr() {
	return &this->imgVec;
}

void TableModel::resetTable() {
	this->beginResetModel();
	this->imgVec.clear();
	this->dispVec.clear();
	this->iconCache->clear();
	this->rowPos = 0;
	this->endResetModel();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if (orientation == Qt::Orientation::Horizontal) {
		if (role == Qt::ItemDataRole::DisplayRole) {
			if (section == 0)
				return QString("Icon");
			else if (section == 1)
				return QString("Hash");
			else if (section == 2)
				return QString("Name");
			else if (section == 3 && this->type == table_type::TABLE_RESULT)
				return QString("Similarity");
		}
	}
	return QVariant();
}

//void TableModel::suspendData(std::pair<int, int> interval) {
//	this->interval = interval;
//	std::thread suspend_thr([this] {
//		Sleep(700);
//		this->interval = { 0, maximum };
//	});
//	suspend_thr.detach();
//}

//void TableModel::sort(int column, Qt::SortOrder order) {
//	if(this->headerData(2, Qt::Horizontal))
//}

DetailDialog::DetailDialog(QWidget* parent, iop::Comparison* comp, iop::WeightVector* wvec) {
	ui.setupUi(this);
	auto ftoq = [](float f) {
		return QString::fromStdString(gen::format(f));
	};
	parent_ptr = (MainWindow*)parent;
	if (comp) {
		ui.label_srcimg->setText(img::buildImageName(comp->source_dir).c_str());
		ui.label_destimg->setText(img::buildImageName(comp->rhand_dir).c_str());
		ui.lineEdit_gm->setText(ftoq(comp->diff_gradm));
		ui.lineEdit_gd->setText(ftoq(comp->diff_gradd));
		ui.lineEdit_hg->setText(ftoq(comp->diff_hgray));
		ui.lineEdit_hbb->setText(ftoq(comp->diff_hbgrb));
		ui.lineEdit_hbg->setText(ftoq(comp->diff_hbgrg));
		ui.lineEdit_hbr->setText(ftoq(comp->diff_hbgrr));
		ui.lineEdit_hhh->setText(ftoq(comp->diff_hhsvh));
		ui.lineEdit_hhs->setText(ftoq(comp->diff_hhsvs));
		ui.lineEdit_hhv->setText(ftoq(comp->diff_hhsvv));
		ui.lineEdit_hd->setText(ftoq(comp->diff_hashd));
		ui.lineEdit_hp->setText(ftoq(comp->diff_hashp));
		ui.lineEdit_ed->setText(ftoq(comp->euc_dist));
	}

	if (wvec) {
		ui.lineEdit_gm_2->setText(ftoq(wvec->wv_grad[0]));
		ui.lineEdit_gd_2->setText(ftoq(wvec->wv_grad[1]));
		ui.lineEdit_hg_2->setText(ftoq(wvec->w_hgray));
		ui.lineEdit_hbb_2->setText(ftoq(wvec->wv_hbgr[0]));
		ui.lineEdit_hbg_2->setText(ftoq(wvec->wv_hbgr[1]));
		ui.lineEdit_hbr_2->setText(ftoq(wvec->wv_hbgr[2]));
		ui.lineEdit_hhh_2->setText(ftoq(wvec->wv_hhsv[0]));
		ui.lineEdit_hhs_2->setText(ftoq(wvec->wv_hhsv[1]));
		ui.lineEdit_hhv_2->setText(ftoq(wvec->wv_hhsv[2]));
		ui.lineEdit_hd_2->setText(ftoq(wvec->wv_hash[0]));
		ui.lineEdit_hp_2->setText(ftoq(wvec->wv_hash[1]));
	}

	current_comp = comp;
}

SortDialog::SortDialog(QWidget* parent, iop::FeatureVector* fv, iop::WeightVector* wv) {
	ui.setupUi(this);
	ui.lineEdit_2->setText("100");
	
	ui.checkBox_B_3->setVisible(false);
	ui.checkBox_G_3->setVisible(false);
	ui.checkBox_R_3->setVisible(false);

	ui.stackedWidget->setCurrentIndex(0);
	ui.stackedWidget_fbin->setCurrentIndex(0);
	ui.stackedWidget_sbin->setCurrentIndex(0);
	ui.stackedWidget_tbin->setCurrentIndex(0);

	ui.stackedWidget_fw->setCurrentIndex(0);
	ui.stackedWidget_sw->setCurrentIndex(0);
	ui.stackedWidget_tw->setCurrentIndex(0);

	ui.stackedWidget_bgr->setCurrentIndex(0);

	checkbox_vec = { ui.checkBox_sorthistg, ui.checkBox_sorthistb, ui.checkBox_sorthisth };

	checkbox_vec_bgr = { {ui.checkBox_B_3, ui.checkBox_G_3, ui.checkBox_R_3}, {ui.checkBox_B, ui.checkBox_G, ui.checkBox_R},
		{ui.checkBox_B_2, ui.checkBox_G_2, ui.checkBox_R_2} };

	lineedit_hist_g = { ui.lineEdit_weighthistf, ui.lineEdit_weighthists, ui.lineEdit_weighthistt };
	lineedit_hist_bgr = { ui.lineEdit_weighthistf_1, ui.lineEdit_weighthists_1, ui.lineEdit_weighthistt_1 };
	lineedit_hist_hsv = { ui.lineEdit_weighthistf_2, ui.lineEdit_weighthists_2, ui.lineEdit_weighthistt_2 };

	lineedit_hist_vec.push_back(lineedit_hist_g);
	lineedit_hist_vec.push_back(lineedit_hist_bgr);
	lineedit_hist_vec.push_back(lineedit_hist_hsv);

	slider_hist_g = { ui.horizontalSlider_fbin, ui.horizontalSlider_sbin, ui.horizontalSlider_tbin };
	slider_hist_bgr = { ui.horizontalSlider_fbin_1, ui.horizontalSlider_sbin_1, ui.horizontalSlider_tbin_1 };
	slider_hist_hsv = { ui.horizontalSlider_fbin_2, ui.horizontalSlider_sbin_2, ui.horizontalSlider_tbin_2 };

	slider_hist_vec.push_back(slider_hist_g);
	slider_hist_vec.push_back(slider_hist_bgr);
	slider_hist_vec.push_back(slider_hist_hsv);

	initlineedit_vec = {ui.lineEdit_18, ui.lineEdit_19, ui.lineEdit_gradirbin, 
		ui.lineEdit_gramagbin, ui.lineEdit_weightgradir, ui.lineEdit_weightgramag };
	initlineedit_vec.insert(initlineedit_vec.end(), lineedit_hist_vec[0].begin(), lineedit_hist_vec[0].end());
	initlineedit_vec.insert(initlineedit_vec.end(), lineedit_hist_vec[1].begin(), lineedit_hist_vec[1].end());
	initlineedit_vec.insert(initlineedit_vec.end(), lineedit_hist_vec[2].begin(), lineedit_hist_vec[2].end());

	QDoubleValidator* validator = new QDoubleValidator(0.0001, 100.0, 1000);
	ui.lineEdit_2->setValidator(validator);
	for (int i = 0; i < initlineedit_vec.size(); i++) {
		initlineedit_vec[i]->setValidator(validator);
		initlineedit_vec[i]->setText("0");
	}

	initlineedit_vec.erase(initlineedit_vec.begin() + 2, initlineedit_vec.begin() + 4);
	
	QObject MWObject;
	parent_ptr = (MainWindow*)parent;
	MWObject.connect(ui.buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(buildFeatureVector()));
	MWObject.connect(ui.checkBox_B_3, &QCheckBox::toggled, [&](bool enable) {enableBGR(enable, 0, 0); });
	MWObject.connect(ui.checkBox_G_3, &QCheckBox::toggled, [&](bool enable) {enableBGR(enable, 0, 1); });
	MWObject.connect(ui.checkBox_R_3, &QCheckBox::toggled, [&](bool enable) {enableBGR(enable, 0, 2); });
	MWObject.connect(ui.checkBox_B, &QCheckBox::toggled, [&](bool enable) {enableBGR(enable, 1, 0); });
	MWObject.connect(ui.checkBox_G, &QCheckBox::toggled, [&](bool enable) {enableBGR(enable, 1, 1); });
	MWObject.connect(ui.checkBox_R, &QCheckBox::toggled, [&](bool enable) {enableBGR(enable, 1, 2); });
	MWObject.connect(ui.checkBox_B_2, &QCheckBox::toggled, [&](bool enable) {enableBGR(enable, 2, 0); });
	MWObject.connect(ui.checkBox_G_2, &QCheckBox::toggled, [&](bool enable) {enableBGR(enable, 2, 1); });
	MWObject.connect(ui.checkBox_R_2, &QCheckBox::toggled, [&](bool enable) {enableBGR(enable, 2, 2); });
	MWObject.connect(ui.checkBox_sortHash, SIGNAL(toggled(bool)), this, SLOT(enableHash(bool)));
	MWObject.connect(ui.checkBox_sorthistg, SIGNAL(toggled(bool)), this, SLOT(enableHistG(bool)));
	MWObject.connect(ui.checkBox_sorthistb, SIGNAL(toggled(bool)), this, SLOT(enableHistB(bool)));
	MWObject.connect(ui.checkBox_sorthisth, SIGNAL(toggled(bool)), this, SLOT(enableHistH(bool)));
	MWObject.connect(ui.checkBox_sortEdge, SIGNAL(toggled(bool)), this, SLOT(enableEdge(bool)));
	MWObject.connect(ui.checkBox, SIGNAL(toggled(bool)), this, SLOT(equaliseWeights(bool)));
	MWObject.connect(ui.comboBox_histFlag, SIGNAL(currentIndexChanged(int)), this, SLOT(displayButtons_BGR(int)));
	for (int i = 0; i < initlineedit_vec.size(); i++) {
		MWObject.connect(initlineedit_vec[i], SIGNAL(textChanged(QString)), this,
			SLOT(remainingPercentage(QString)));
	}
	for(auto &i : slider_hist_vec)
		for(auto &j : i)
			MWObject.connect(j, &QSlider::sliderMoved,
				[&](int value) {
					QToolTip::showText(QCursor::pos(), QString("%1").arg(value), nullptr);
				});
	
	if (fv && wv)
		initialiseWeights(fv, wv);
}

void SortDialog::equaliseWeights(bool enable) {
	if (enable) {
		iop::WeightVector wv(true);
		auto ftoq = [](float f) {
			return QString::fromStdString(gen::format(f));
		};
		ui.lineEdit_18->setText(ftoq(wv.wv_hash[0] * 100));
		ui.lineEdit_19->setText(ftoq(wv.wv_hash[1] * 100));
		ui.lineEdit_weightgramag->setText(ftoq(wv.wv_grad[0] * 100));
		ui.lineEdit_weightgradir->setText(ftoq(wv.wv_grad[1] * 100));
		ui.lineEdit_weighthistf->setText(ftoq(wv.w_hgray * 100));
		ui.lineEdit_weighthists->setText(ftoq(0));
		ui.lineEdit_weighthistt->setText(ftoq(0));
		ui.lineEdit_weighthistf_1->setText(ftoq(wv.wv_hbgr[0] * 100));
		ui.lineEdit_weighthists_1->setText(ftoq(wv.wv_hbgr[1] * 100));
		ui.lineEdit_weighthistt_1->setText(ftoq(wv.wv_hbgr[2] * 100));
		ui.lineEdit_weighthistf_2->setText(ftoq(wv.wv_hhsv[0] * 100));
		ui.lineEdit_weighthists_2->setText(ftoq(wv.wv_hhsv[1] * 100));
		ui.lineEdit_weighthistt_2->setText(ftoq(wv.wv_hhsv[2] * 100));
		ui.horizontalSlider_fbin->setValue(10);
		ui.horizontalSlider_sbin->setValue(0);
		ui.horizontalSlider_tbin->setValue(0);
		ui.horizontalSlider_fbin_1->setValue(5);
		ui.horizontalSlider_sbin_1->setValue(5);
		ui.horizontalSlider_tbin_1->setValue(5);
		ui.horizontalSlider_fbin_2->setValue(5);
		ui.horizontalSlider_sbin_2->setValue(5);
		ui.horizontalSlider_tbin_2->setValue(5);
		for (int i = 0; i < lineedit_hist_vec.size(); i++) {
			for (int j = 0; j < lineedit_hist_vec[i].size(); j++) {
					checkbox_vec[i]->setChecked(true);
					checkbox_vec_bgr[i][j]->setChecked(true);
			}
		}
		ui.comboBox_edgeFlag->setCurrentIndex(0);
		ui.lineEdit_gradirbin->setText(ftoq(8));
		ui.lineEdit_gramagbin->setText(ftoq(10));
		ui.checkBox_sorthistg->setChecked(true);
		ui.checkBox_sorthistb->setChecked(true);
		ui.checkBox_sorthisth->setChecked(true);
		ui.checkBox_sortEdge->setChecked(true);
		ui.checkBox_sortHash->setChecked(true);
		emit displayButtons_BGR(ui.comboBox_histFlag->currentIndex());
		emit remainingPercentage("", true);
		ui.checkBox->setChecked(true);
	}
}

void SortDialog::initialiseWeights(iop::FeatureVector* fv, iop::WeightVector* wv) {
	auto ftoq = [](float f) {
		return QString::fromStdString(gen::format(f));
	};
	ui.lineEdit_18->setText(ftoq(wv->wv_hash[0] * 100));
	ui.lineEdit_19->setText(ftoq(wv->wv_hash[1] * 100));
	ui.lineEdit_weightgramag->setText(ftoq(wv->wv_grad[0] * 100));
	ui.lineEdit_weightgradir->setText(ftoq(wv->wv_grad[1] * 100));
	ui.lineEdit_weighthistf->setText(ftoq(wv->w_hgray * 100));
	ui.lineEdit_weighthists->setText(ftoq(0));
	ui.lineEdit_weighthistt->setText(ftoq(0));
	ui.lineEdit_weighthistf_1->setText(ftoq(wv->wv_hbgr[0] * 100));
	ui.lineEdit_weighthists_1->setText(ftoq(wv->wv_hbgr[1] * 100));
	ui.lineEdit_weighthistt_1->setText(ftoq(wv->wv_hbgr[2] * 100));
	ui.lineEdit_weighthistf_2->setText(ftoq(wv->wv_hhsv[0] * 100));
	ui.lineEdit_weighthists_2->setText(ftoq(wv->wv_hhsv[1] * 100));
	ui.lineEdit_weighthistt_2->setText(ftoq(wv->wv_hhsv[2] * 100));
	ui.horizontalSlider_fbin->setValue(fv->hist_gray->getVariablesFloat()[1]);
	ui.horizontalSlider_sbin->setValue(0);
	ui.horizontalSlider_tbin->setValue(0);
	ui.horizontalSlider_fbin_1->setValue(fv->hist_bgr->getVariablesFloat()[1]);
	ui.horizontalSlider_sbin_1->setValue(fv->hist_bgr->getVariablesFloat()[2]);
	ui.horizontalSlider_tbin_1->setValue(fv->hist_bgr->getVariablesFloat()[3]);
	ui.horizontalSlider_fbin_2->setValue(fv->hist_hsv->getVariablesFloat()[1]);
	ui.horizontalSlider_sbin_2->setValue(fv->hist_hsv->getVariablesFloat()[2]);
	ui.horizontalSlider_tbin_2->setValue(fv->hist_hsv->getVariablesFloat()[3]);

	for (int i = 0; i < lineedit_hist_vec.size(); i++) {
		checkbox_vec[i]->setChecked(false);
		for (int j = 0; j < lineedit_hist_vec[i].size(); j++) {
			checkbox_vec_bgr[i][j]->setChecked(false);
			auto curr = lineedit_hist_vec[i][j];
			if (curr->text() != "0") {
				checkbox_vec[i]->setChecked(true);
				checkbox_vec_bgr[i][j]->setChecked(true);
			}
		}
	}

	ui.comboBox_edgeFlag->setCurrentIndex(fv->edge->getEdgeFlag());
	ui.lineEdit_gradirbin->setText(ftoq(fv->edge->getComparisonValues()[1]));
	ui.lineEdit_gramagbin->setText(ftoq(fv->edge->getComparisonValues()[2]));

	if (fv->edge->getComparisonValues()[1] != 0 || fv->edge->getComparisonValues()[2] != 0) {
		ui.checkBox_sortEdge->setChecked(true);
		emit enableEdge(true);
	}
	if (fv->perc_hash->getSelectHash().first != false || fv->perc_hash->getSelectHash().second != false) {
		ui.checkBox_sortHash->setChecked(true);
		emit enableHash(true);
	}
	emit displayButtons_BGR(ui.comboBox_histFlag->currentIndex());
	emit remainingPercentage("", true);
	ui.checkBox->setChecked(false);
}

void SortDialog::enableBGR(bool enable, int combobox, int index) {
	if (combobox == 0);
	else {
		slider_hist_vec[combobox][index]->setEnabled(enable && checkbox_vec[combobox]->isChecked());
		lineedit_hist_vec[combobox][index]->setEnabled(enable && checkbox_vec[combobox]->isChecked());
	}
	emit remainingPercentage("", true);
}

void SortDialog::remainingPercentage(QString sent, bool disabled) {
	float initial = ui.lineEdit_2->text().toFloat();
	
	float currentPerc = 0;
	for (auto i : initlineedit_vec) {
		if (i->isEnabled())
			currentPerc += i->text().toFloat();
	}

	ui.lineEdit_2->setText(QString::fromStdString(gen::format(100 - currentPerc)));
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
	ui.stackedWidget_bgr->setCurrentIndex(0);
	ui.stackedWidget_fbin->setCurrentIndex(0);
	ui.stackedWidget_sbin->setCurrentIndex(0);
	ui.stackedWidget_tbin->setCurrentIndex(0);
	slider_hist_g[0]->setEnabled(enable);
	slider_hist_g[1]->setEnabled(false);
	slider_hist_g[2]->setEnabled(false);
	ui.stackedWidget_fw->setCurrentIndex(0);
	ui.stackedWidget_sw->setCurrentIndex(0);
	ui.stackedWidget_tw->setCurrentIndex(0);
	lineedit_hist_g[0]->setEnabled(enable);
	lineedit_hist_g[1]->setEnabled(false);
	lineedit_hist_g[2]->setEnabled(false);
	ui.stackedWidget_fbinlabel->setCurrentIndex(0);
	emit remainingPercentage("g", true);
}

void SortDialog::enableHistB(bool enable) {
	ui.stackedWidget_bgr->setCurrentIndex(1);
	checkbox_vec_bgr[1][0]->setEnabled(enable);
	checkbox_vec_bgr[1][1]->setEnabled(enable);
	checkbox_vec_bgr[1][2]->setEnabled(enable);
	ui.stackedWidget_fbin->setCurrentIndex(1);
	ui.stackedWidget_sbin->setCurrentIndex(1);
	ui.stackedWidget_tbin->setCurrentIndex(1);
	slider_hist_bgr[0]->setEnabled(checkbox_vec_bgr[1][0]->isChecked() && enable);
	slider_hist_bgr[1]->setEnabled(checkbox_vec_bgr[1][1]->isChecked() && enable);
	slider_hist_bgr[2]->setEnabled(checkbox_vec_bgr[1][2]->isChecked() && enable);
	ui.stackedWidget_fw->setCurrentIndex(1);
	ui.stackedWidget_sw->setCurrentIndex(1);
	ui.stackedWidget_tw->setCurrentIndex(1);
	lineedit_hist_bgr[0]->setEnabled(checkbox_vec_bgr[1][0]->isChecked() && enable);
	lineedit_hist_bgr[1]->setEnabled(checkbox_vec_bgr[1][1]->isChecked() && enable);
	lineedit_hist_bgr[2]->setEnabled(checkbox_vec_bgr[1][2]->isChecked() && enable);
	ui.stackedWidget_fbinlabel->setCurrentIndex(0);
	emit remainingPercentage("b", true);
}

void SortDialog::enableHistH(bool enable) {
	ui.stackedWidget_bgr->setCurrentIndex(2);
	checkbox_vec_bgr[2][0]->setEnabled(enable);
	checkbox_vec_bgr[2][1]->setEnabled(enable);
	checkbox_vec_bgr[2][2]->setEnabled(enable);
	ui.stackedWidget_fbin->setCurrentIndex(2);
	ui.stackedWidget_sbin->setCurrentIndex(2);
	ui.stackedWidget_tbin->setCurrentIndex(2);
	slider_hist_hsv[0]->setEnabled(checkbox_vec_bgr[2][0]->isChecked() && enable);
	slider_hist_hsv[1]->setEnabled(checkbox_vec_bgr[2][1]->isChecked() && enable);
	slider_hist_hsv[2]->setEnabled(checkbox_vec_bgr[2][2]->isChecked() && enable);
	ui.stackedWidget_fw->setCurrentIndex(2);
	ui.stackedWidget_sw->setCurrentIndex(2);
	ui.stackedWidget_tw->setCurrentIndex(2);
	lineedit_hist_hsv[0]->setEnabled(checkbox_vec_bgr[2][0]->isChecked() && enable);
	lineedit_hist_hsv[1]->setEnabled(checkbox_vec_bgr[2][1]->isChecked() && enable);
	lineedit_hist_hsv[2]->setEnabled(checkbox_vec_bgr[2][2]->isChecked() && enable);
	ui.stackedWidget_fbinlabel->setCurrentIndex(1);
	emit remainingPercentage("h", true);
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
	auto qtof = [](QString q) {
		return q.toFloat();
	};
	iop::WeightVector wv(true);
	wv.wv_hash[0] = ui.lineEdit_18->isEnabled() ? ui.lineEdit_18->text().toFloat() / 100 : 0;
	wv.wv_hash[1] = ui.lineEdit_19->isEnabled() ? ui.lineEdit_19->text().toFloat() / 100 : 0;
	wv.wv_grad[0] = ui.lineEdit_weightgramag->isEnabled() ? ui.lineEdit_weightgramag->text().toFloat() / 100 : 0;
	wv.wv_grad[1] = ui.lineEdit_weightgradir->isEnabled() ? ui.lineEdit_weightgradir->text().toFloat() / 100 : 0;
	wv.w_hgray = lineedit_hist_vec[0][0]->isEnabled() ? qtof(lineedit_hist_vec[0][0]->text()) / 100 : 0;
	delete(wv.wvv_total[1]);
	wv.wvv_total[1] = new vecf{ wv.w_hgray };
	wv.wv_hbgr[0] = lineedit_hist_vec[1][0] ->isEnabled() ? qtof(lineedit_hist_vec[1][0]->text()) / 100 : 0;
	wv.wv_hbgr[1] = lineedit_hist_vec[1][1]->isEnabled() ? qtof(lineedit_hist_vec[1][1]->text()) / 100 : 0;
	wv.wv_hbgr[2] = lineedit_hist_vec[1][2]->isEnabled() ? qtof(lineedit_hist_vec[1][2]->text()) / 100 : 0;
	wv.wv_hhsv[0] = lineedit_hist_vec[2][0]->isEnabled() ? qtof(lineedit_hist_vec[2][0]->text()) / 100 : 0;
	wv.wv_hhsv[1] = lineedit_hist_vec[2][1]->isEnabled() ? qtof(lineedit_hist_vec[2][1]->text()) / 100 : 0;
	wv.wv_hhsv[2] = lineedit_hist_vec[2][2]->isEnabled() ? qtof(lineedit_hist_vec[2][2]->text()) / 100 : 0;

	feat::Histogram hist_g;
	if(checkbox_vec[0]->isChecked())
		hist_g = feat::Histogram(cv::Mat(), HIST_GRAY, slider_hist_vec[0][0]->value());
	else
		hist_g = feat::Histogram(cv::Mat(), HIST_GRAY, 0, 0, 0);

	feat::Histogram hist_bgr;
	if (checkbox_vec[1]->isChecked())
		hist_bgr = feat::Histogram(cv::Mat(), HIST_BGR, checkbox_vec_bgr[1][0]->isChecked() ? slider_hist_vec[1][0]->value() : 0, 
			checkbox_vec_bgr[1][1]->isChecked() ? slider_hist_vec[1][1]->value() : 0, 
			checkbox_vec_bgr[1][2]->isChecked() ? slider_hist_vec[1][2]->value() : 0);
	else
		hist_bgr = feat::Histogram(cv::Mat(), HIST_BGR, 0, 0, 0);

	feat::Histogram hist_hsv;
	if (checkbox_vec[2]->isChecked())
		hist_hsv = feat::Histogram(cv::Mat(), HIST_HSV, checkbox_vec_bgr[2][0]->isChecked() ? slider_hist_vec[2][0]->value() : 0,
			checkbox_vec_bgr[2][1]->isChecked() ? slider_hist_vec[2][1]->value() : 0,
			checkbox_vec_bgr[2][2]->isChecked() ? slider_hist_vec[2][2]->value() : 0);
	else
		hist_hsv = feat::Histogram(cv::Mat(), HIST_HSV, 0, 0, 0);

	feat::Edge edge;
	if (ui.checkBox_sortEdge->isChecked())
		edge = feat::Edge(cv::Mat(), ui.comboBox_edgeFlag->currentIndex(), nullptr,
			-1, ui.lineEdit_gramagbin->text().toFloat(), ui.lineEdit_gradirbin->text().toFloat());
	else
		edge = feat::Edge(cv::Mat(), 0, nullptr, -1, 0, 0);
	
	feat::Hash hash;
	if (ui.checkBox_sortHash->isChecked())
		hash = feat::Hash(cv::Mat(), std::make_pair(true, true));
	else
		hash = feat::Hash(cv::Mat(), std::make_pair(false, false));

	iop::FeatureVector fv(nullptr, &edge, &hist_g, &hist_bgr, &hist_hsv, &hash);

	emit parent_ptr->setFeatureVector(fv, wv);
}

void SortDialog::displayButtons_BGR(int state) {
	if (state == 0) {
		enableHistG(ui.checkBox_sorthistg->isChecked());
		ui.stackedWidget->setCurrentIndex(0);
	}
	else if (state == 1) {
		ui.stackedWidget->setCurrentIndex(1);
		enableHistB(ui.checkBox_sorthistb->isChecked());
	}
	else if (state == 2) {
		ui.stackedWidget->setCurrentIndex(2);
		enableHistH(ui.checkBox_sorthisth->isChecked());
	}
}

MainWindow::MainWindow(dbop::Database dbObj, QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);

	setIcons();
	screenSize = new QSize(this->geometry().width(), this->geometry().height());

	QResource::registerResource("Resource.rcc");

	ui.mainTabWidget->removeTab(2);
	ui.comboBox_kernelFlag->setCurrentIndex(3);

	mw_dbPtr = &dbObj;
	lnkr::deleteFromSimAndWV(); 

	qDB = QSqlDatabase::addDatabase("QSQLITE");
	qDB.setDatabaseName("bitirme.db");

	if (!qDB.open())
		showError(qDB.lastError());
	qDB.exec("PRAGMA synchronous = OFF");
	qDB.exec("PRAGMA journal_mode = MEMORY");

	resultModel = new TableModel(table_type::TABLE_RESULT, {});
	
	QSqlQuery query(qDB);
	if (query.prepare("SELECT DISTINCT b.dir, b.hash, b.name, similarity.similarity"
		" FROM image a, image b"
		" INNER JOIN similarity"
		" ON similarity.srchash = a.hash and similarity.desthash = b.hash"))
		query.exec();
	else
		throw std::exception("Failed initiating image table.");
	
	proxyModel_result = new SortProxyModel(ui.resultTableView);
	proxyModel_result->setDynamicSortFilter(true);
	proxyModel_result->setSourceModel(resultModel);
	ui.resultTableView->setModel(proxyModel_result);
	ui.resultTableView->setSortingEnabled(true);

	ui.resultTableView->setColumnHidden(1, true);
	
	ui.resultTableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	ui.resultTableView->verticalHeader()->setDefaultSectionSize(100);
	ui.resultTableView->verticalHeader()->setVisible(false);

	ui.resultTableView->horizontalHeader()->setModel(resultModel);
	ui.resultTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	ui.resultTableView->verticalHeader()->setDefaultSectionSize(100);
	ui.resultTableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
	ui.resultTableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);

	ui.resultTableView->setContextMenuPolicy(Qt::CustomContextMenu);

	/*"SELECT DISTINCT a.dir, a.hash, a.name"
		" FROM image a"*/

	
	auto imageVec = lnkr::getImageTable();
	std::vector<std::pair<img::Image, float>> parVec;
	for (auto i : imageVec)
		parVec.push_back({ i, -1 });
	mainModel = new TableModel(table_type::TABLE_MAIN, parVec);
	/*for (auto i : imageVec)
		mainModel->insertImage(i);*/
	proxyModel_main = new SortProxyModel(ui.mainTableView);
	proxyModel_main->setDynamicSortFilter(true);
	proxyModel_main->setSourceModel(mainModel);
	ui.mainTableView->setModel(proxyModel_main);
	ui.mainTableView->setSortingEnabled(true);
	
	ui.mainTableView->setColumnHidden(1, true);

	ui.mainTableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	ui.mainTableView->verticalHeader()->setDefaultSectionSize(100);
	ui.mainTableView->verticalHeader()->setVisible(false);

	ui.mainTableView->horizontalHeader()->setModel(mainModel);
	ui.mainTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	ui.mainTableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

	ui.mainTableView->setContextMenuPolicy(Qt::CustomContextMenu);

	ui.checkBox_icon->setChecked(false);
	showIcons(false);

	ui.label_imgSrc->setBackgroundRole(QPalette::Base);
	//ui.label_imgSrc->setScaledContents(true);

	QDoubleValidator* validator = new QDoubleValidator(0.0001, 100.0, 1000);
	ui.lineEdit_alpha->setValidator(validator);
	ui.lineEdit_alpha->setText("0.04");
	ui.lineEdit_gauss->setValidator(validator);
	ui.lineEdit_gauss->setText("5");
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

	ui.stackedWidget->setHidden(true);
	ui.stackedWidget_2->setHidden(true);

	QObject MWObject;
	MWObject.connect(ui.pushButton_switch, SIGNAL(clicked()), this, SLOT(switchTables()));
	MWObject.connect(ui.pushButton_srcImgLabel, SIGNAL(clicked()), this, SLOT(openImageLabel()));
	MWObject.connect(ui.comboBox_edgeFlag, SIGNAL(currentIndexChanged(int)), this, SLOT(enableCanny(int)));
	MWObject.connect(ui.pushButton_loadImgs, SIGNAL(clicked()), this, SLOT(openList()));
	MWObject.connect(ui.pushButton_dispInDet, SIGNAL(clicked()), this, SLOT(displayFeature()));
	MWObject.connect(ui.comboBox_histFlag, SIGNAL(currentIndexChanged(int)), this, SLOT(displayButtons_BGR()));
	MWObject.connect(ui.mainTableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customMenuRequested_main(QPoint)));
	MWObject.connect(ui.resultTableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customMenuRequested_result(QPoint)));
	MWObject.connect(ui.toolButton_settings, SIGNAL(clicked()), this, SLOT(openSortDialog()));
	MWObject.connect(ui.checkBox_icon, SIGNAL(toggled(bool)), this, SLOT(showIcons(bool)));
	MWObject.connect(ui.pushButton_sort, SIGNAL(clicked()), this, SLOT(compareMain()));
	MWObject.connect(ui.comboBox_statSrc, SIGNAL(currentIndexChanged(int)), this, SLOT(statsTab_enableBGR_src(int)));
	MWObject.connect(ui.comboBox_statDest, SIGNAL(currentIndexChanged(int)), this, SLOT(statsTab_enableBGR_dest(int)));
	MWObject.connect(ui.horizontalSlider_fbin, &QSlider::sliderMoved,
		[&](int value) {
			QToolTip::showText(QCursor::pos(), QString("%1").arg(value), nullptr);
		});
	MWObject.connect(ui.horizontalSlider_sbin, &QSlider::sliderMoved,
		[&](int value) {
			QToolTip::showText(QCursor::pos(), QString("%1").arg(value), nullptr);
		});
	MWObject.connect(ui.horizontalSlider_sbin, &QSlider::sliderMoved,
		[&](int value) {
			QToolTip::showText(QCursor::pos(), QString("%1").arg(value), nullptr);
		});
	
	ui.mainTabWidget->setCurrentIndex(1);
	ui.mainTabWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow(){
	lnkr::deleteFromSimAndWV();
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
}

void MainWindow::openSortDialog() {
	SortDialog sortDialog(this, currentfv, currentwv);
	sortDialog.exec();
}

void MainWindow::openDetailDialog(iop::Comparison* comp, iop::WeightVector* wvec) {
	if (!wvec) {
		throw gen::Warning("Define weight vector first.");
		return;
	}
	DetailDialog detailDialog(this, comp, wvec);
	detailDialog.exec();
}

void MainWindow::showError(const QSqlError& err) {
	QMessageBox::critical(this, "Unable to initialize Database",
		"Error initializing database: " + err.text());
}

void MainWindow::showError(QString err) {
	QMessageBox::critical(this, "ERROR or as we intellectuals call it, EXCEPTION", "Error! \n" + err);
}

void MainWindow::showWarning(QString err) {
	QMessageBox::warning(this, "Warning", err);
}

Ui::MainWindow MainWindow::getUI() {
	return ui;
}

void MainWindow::setFeatureVector(iop::FeatureVector returnfv, iop::WeightVector returnwv) {
	if (currentfv) {
		delete(currentfv);
		currentfv = nullptr;
	}
	currentfv = new iop::FeatureVector(returnfv);

	if (currentwv) {
		delete(currentwv);
		currentwv = nullptr;
	}
	currentwv = new iop::WeightVector(returnwv);
}

void MainWindow::showIcons(bool show) {
	if (show) {
		int pos_main = ui.mainTableView->indexAt(ui.mainTableView->rect().topLeft()).row();
		proxyModel_main->suspendData({ pos_main - 5, pos_main + 15 });
		int pos_res = ui.resultTableView->indexAt(ui.resultTableView->rect().topLeft()).row();
		proxyModel_result->suspendData({ pos_res - 5, pos_res + 15 });
		ui.mainTableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
		ui.mainTableView->verticalHeader()->setDefaultSectionSize(100);
		ui.resultTableView->setColumnHidden(0, false);
		ui.mainTableView->setColumnHidden(0, false);
		resultModel->setIconColumnHidden(false);
		mainModel->setIconColumnHidden(false);
	}
	else {
		ui.mainTableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
		ui.mainTableView->verticalHeader()->setDefaultSectionSize(1);
		ui.resultTableView->setColumnHidden(0, true);
		ui.mainTableView->setColumnHidden(0, true);
		resultModel->setIconColumnHidden(true);
		mainModel->setIconColumnHidden(true);
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
	float width = -1, height = -1;
	const float frame_ratio = ui.mainTabWidget->currentIndex() == 0 ?
		static_cast<float>(3) / 14 : static_cast<float>(7) / 20;
	frame->update();
	float frmwid = frame->width();
	float frmhgt = frame->height();
	float imgwid = imlabel->width();
	float imghgt = imlabel->height();
	/*if (abs(imlabel->width() - frame->width()) < frame->width() * 2 / 100
		&& abs(imlabel->height() - frame->height()) < frame->height() * 2 / 100)
		return;*/
	if (image.width() >= image.height()) {
		width = screenSize->width() * frame_ratio;
		height = width * image.height() / image.width() 
			> frame->height() ? frame->height()
			: width * image.height() / image.width();
		width = height * image.width() / image.height();
		imlabel->setMaximumHeight(height);
		imlabel->setMaximumWidth(width);
	}
	else if (image.height() >= image.width()) {
		height = frame->height() > screenSize->height() * frame_ratio ? screenSize->height() * frame_ratio : frame->height();
		width = height * image.width() / image.height();
		imlabel->setMaximumWidth(width);
		imlabel->setMaximumHeight(height);
	}
	image = image.scaled(width, height, Qt::KeepAspectRatio);
}

void MainWindow::displayFeature(bool emitted, int index) {
	emitted ? index = ui.tabWidget_comparison->currentIndex() : index = index;
	if (!source_img) {
		throw gen::Warning("Source image must be picked first.");
		return;
	}
	else if (!dest_img && index != 4) {
		throw gen::Warning("Comparison image(right hand) must be picked first.");
		return;
	}
	img::Image srcImg = *source_img;
	img::Image destImg = *dest_img;

	if (index == 0) {
		displayHash(&srcImg);
	}
	else if (index == 1) {
		ui.mainTabWidget->setCurrentIndex(1);
		QImage srcImgScaled = cvMatToQImage(srcImg.getImageMat());
		QImage destImgScaled = cvMatToQImage(destImg.getImageMat());
		scaleImage(srcImgScaled, ui.label_imgSrcBig, ui.frame_src);
		scaleImage(destImgScaled, ui.label_imgDestBig, ui.frame_dest);
		ui.label_imgSrcBig->setPixmap(QPixmap::fromImage(srcImgScaled));
		ui.label_imgDestBig->setPixmap(QPixmap::fromImage(destImgScaled));
		displayHistogram(&srcImg, true, emitted ? nullptr : &curr_feat_src.feature.histogram);
		displayHistogram(&destImg, false, emitted ? nullptr : &curr_feat_dest.feature.histogram);
	}
	else if (index == 2) {
		ui.mainTabWidget->setCurrentIndex(1);
		QImage srcImgScaled = cvMatToQImage(srcImg.getImageMat());
		QImage destImgScaled = cvMatToQImage(destImg.getImageMat());
		scaleImage(srcImgScaled, ui.label_imgSrcBig, ui.frame_src);
		scaleImage(destImgScaled, ui.label_imgDestBig, ui.frame_dest);
		ui.label_imgSrcBig->setPixmap(QPixmap::fromImage(srcImgScaled));
		ui.label_imgDestBig->setPixmap(QPixmap::fromImage(destImgScaled));
		displayEdge(&srcImg, true, emitted ? nullptr : &curr_feat_src.feature.edge);
		displayEdge(&destImg, false, emitted ? nullptr : &curr_feat_dest.feature.edge);
	}
	else if (index == 3) {
		ui.mainTabWidget->setCurrentIndex(1);
		QImage srcImgScaled = cvMatToQImage(srcImg.getImageMat());
		QImage destImgScaled = cvMatToQImage(destImg.getImageMat());
		scaleImage(srcImgScaled, ui.label_imgSrcBig, ui.frame_src);
		scaleImage(destImgScaled, ui.label_imgDestBig, ui.frame_dest);
		ui.label_imgSrcBig->setPixmap(QPixmap::fromImage(srcImgScaled));
		ui.label_imgDestBig->setPixmap(QPixmap::fromImage(destImgScaled));
		displayCorner(&srcImg, true, emitted ? nullptr : &curr_feat_src.feature.corner);
		displayCorner(&destImg, false, emitted ? nullptr : &curr_feat_dest.feature.corner);
	}
	else if (index == 4) {
		ui.mainTabWidget->setCurrentIndex(1);
		if (source_img) {
			QImage srcImgScaled = cvMatToQImage(srcImg.getImageMat());
			scaleImage(srcImgScaled, ui.label_imgSrcBig, ui.frame_src);
			ui.label_imgSrcBig->setPixmap(QPixmap::fromImage(srcImgScaled));
		}
		if (dest_img) {
			QImage destImgScaled = cvMatToQImage(destImg.getImageMat());
			scaleImage(destImgScaled, ui.label_imgDestBig, ui.frame_dest);
			ui.label_imgDestBig->setPixmap(QPixmap::fromImage(destImgScaled));
		}		
		displayStatistics();
	}
}

void MainWindow::displayHash(img::Image* src) {
	auto index = ui.comboBox->currentIndex();
	std::pair<bool,bool> selectHash;
	if (index == 0)
		selectHash = std::make_pair(true, false);
	else if (index == 1)
		selectHash = std::make_pair(false, true);
	else
		throw gen::Warning("Illegal hash flag.");

	string srcHashStr, destHashStr;
	if (!source_img) {
		//throw gen::Warning("Unable to locate source image. Load image to source first.");
		//return;
	}
	else {
		auto srcHash = feat::Hash(source_img->getImageMat(), selectHash);
		auto srcHashPair = srcHash.getHashVariables();
		srcHashStr = selectHash.first ? srcHashPair.first.to_string() : srcHashPair.second.to_string();
		ui.label_imHash->setText(srcHashStr.c_str());
		curr_feat_src.destroyCurrent();
		new(&curr_feat_src.feature.hash) feat::Hash;
		curr_feat_src.feature.hash = srcHash;
		curr_feat_src.index = 0;
	}

	if (!dest_img) {		
		//throw gen::Warning("Unable to locate comparison image. Load image to compare first.");
		//return;
	}
	else {
		auto destHash = feat::Hash(dest_img->getImageMat(), selectHash);
		auto destHashPair = destHash.getHashVariables();
		destHashStr = selectHash.first ? destHashPair.first.to_string() : destHashPair.second.to_string();
		ui.label_destHash->setText(destHashStr.c_str());
	}

	if (source_img && dest_img) {
		QString line = "";
		QString html_red = "<font color = \"Crimson\">";
		QString html_green = "<font color = \"Green\">";
		QString html_end = "</font>";
		for (int i = 0; i < srcHashStr.size(); i++) {
			if (srcHashStr[i] != destHashStr[i])
				line = line % html_red % "1" % html_end;
			else
				line = line % html_green % "0" % html_end;
		}
		ui.label_diffHash->setText(line);
	}
}

void MainWindow::displayHistogram(img::Image* src, bool source, feat::Histogram* hist){
	QList<int> histVals;
	if(!hist)
		histVals = prepareHistogram();
	
	feat::Histogram* srcHist = hist ? new feat::Histogram(*hist) : lnkr::setHistogram(src, histVals[0], histVals[1], histVals[2], histVals[3]);
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

	if (srcHist->getVariablesFloat()[0] == HIST_DATA) {
		QVector<double> data, keys;
		for (int i = 0; i < histPtr.total(); i++) {
			keys.push_back(static_cast<float>(100) / static_cast<float>(histPtr.total()) * i);
			data.push_back(static_cast<double>(histPtr.at<float>(i)));
		}

		int pc = plotSrc->plottableCount();
		for (int i = 0; i < pc; i++) {
			plotSrc->removeGraph(plotSrc->graph(0));
			plotSrc->removePlottable(plotSrc->plottable(0));
		}
		plotSrc->replot();

		QCPBars* barSrc = new QCPBars(plotSrc->xAxis, plotSrc->yAxis);
		//QCPGraph* graphPtr = new QCPGraph(plotSrc->xAxis, plotSrc->yAxis);
		barSrc->setData(keys, data);
		barSrc->setName("Data Histogram");
		barSrc->setPen(QPen(QColor(Qt::black)));
		plotSrc->xAxis->setLabel("Histogram bins");
		plotSrc->yAxis->setLabel("Number of value");
		plotSrc->yAxis->setRange(0, max + max * 0.10);
		plotSrc->xAxis->setRange(0, 100 + 100 * 0.10);
		plotSrc->replot();
	}

	else if (histPtr.channels() == 1) {
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

	if (source) {
		curr_feat_src.destroyCurrent();
		new(&curr_feat_src.feature.histogram) feat::Histogram();
		curr_feat_src.feature.histogram = *srcHist;
		curr_feat_src.index = 1;
	}
	else {
		curr_feat_dest.destroyCurrent();
		new(&curr_feat_dest.feature.histogram) feat::Histogram();
		curr_feat_dest.feature.histogram = *srcHist;
		curr_feat_dest.index = 1;
	}

	delete(srcHist);
}

void MainWindow::displayEdge(img::Image* src, bool source, feat::Edge* edge) {
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
		case KERNEL_ISOBEL:
			kernelX = feat::isobelX;
			kernelY = feat::isobelY;
			break;
		default:
			throw gen::Warning("Illegal kernel flag.");
			return;
		}
		feat::Edge::Canny* canny = new feat::Edge::Canny(edgeVals[1], edgeVals[4], edgeVals[3], edgeVals[2], kernelX, kernelY);
		srcEdge = edge ? new feat::Edge(*edge) : lnkr::setEdge(src, edgeVals[0], canny);
	}

	else {
		srcEdge = edge ? new feat::Edge(*edge) : lnkr::setEdge(src, edgeVals[0]);
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

	if (source) {
		curr_feat_src.destroyCurrent();
		new(&curr_feat_src.feature.edge) feat::Edge();
		curr_feat_src.feature.edge = *srcEdge;
		curr_feat_src.index = 2;
	}
	else {
		curr_feat_dest.destroyCurrent();
		new(&curr_feat_dest.feature.edge) feat::Edge();
		curr_feat_dest.feature.edge = *srcEdge;
		curr_feat_dest.index = 2;
	}

	delete(srcEdge);
}
//cornerFlag, radius, squareSize, alpha, sigmai, sigmad
void MainWindow::displayCorner(img::Image* src, bool source, feat::Corner* corner) {
	QList<float> cornerVals = prepareCorner();
	feat::Corner* srcCorner;

	feat::Corner::Harris* harris = new feat::Corner::Harris(cornerVals[1], cornerVals[2], cornerVals[4], cornerVals[5], cornerVals[3]);
	srcCorner = corner ? new feat::Corner(*corner) : lnkr::setCorner(src, *harris, cornerVals[0], cornerVals[6], 0);

	QImage image = cvMatToQImage(srcCorner->getCornerMarkedMat());
	if (source) {
		QLabel* imlabel = ui.label_derSrcBig;
		scaleImage(image, imlabel, ui.stackedWidget_src);
		imlabel->setPixmap(QPixmap::fromImage(image));
	}
	else {
		QLabel* imlabel = ui.label_derDestBig;
		scaleImage(image, imlabel, ui.stackedWidget_dest);
		imlabel->setPixmap(QPixmap::fromImage(image));
	}

	switchDisplayWidgets(true, source);

	if (source) {
		curr_feat_src.destroyCurrent();
		new(&curr_feat_src.feature.corner) feat::Corner();
		curr_feat_src.feature.corner = *srcCorner;
		curr_feat_src.index = 3;
	}
	else {
		curr_feat_dest.destroyCurrent();
		new(&curr_feat_dest.feature.corner) feat::Corner();
		curr_feat_dest.feature.corner = *srcCorner;
		curr_feat_dest.index = 3;
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
		throw gen::Warning("Zero value sliders. Sliders can't be zero, pick a value.");

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
		throw gen::Warning("Zero value boxes. Canny values can't be zero, pick a value.");

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
		throw gen::Warning("Zero value boxes. Harris values can't be zero, pick a value.");

	return cornerVals;
}

bool MainWindow::loadFiles(const QStringList& fileNames, bool check) {
	std::vector<img::Image> imgVec;

	std::filesystem::path currentPath(fileNames[0].toStdString());
	if (lastpath)
		delete(lastpath);
	lastpath = new QString(currentPath.string().c_str());

	for (int i = 0; i < fileNames.size(); i++) {
		QImageReader reader(fileNames[i]);
		reader.setAutoTransform(true);
		img::Image newImage(QDir::toNativeSeparators(fileNames[i]).toStdString(), check ? cv::IMREAD_COLOR : IMG_EMPTY);
		imgVec.push_back(newImage);
	}

	auto addToDB = [&](std::vector<img::Image> imgVec) {
		int rc = sqlite3_exec(mw_dbPtr->databasePtr(), "BEGIN TRANSACTION;", NULL, NULL, NULL);
		mw_dbPtr->errorCheck(rc, const_cast<char*>(sqlite3_errmsg(mw_dbPtr->databasePtr())));

		for (auto& iter : imgVec) {
			addToMainTableNoRefresh(&iter);
		}

		rc = sqlite3_exec(mw_dbPtr->databasePtr(), "END TRANSACTION;", NULL, NULL, NULL);
		mw_dbPtr->errorCheck(rc, const_cast<char*>(sqlite3_errmsg(mw_dbPtr->databasePtr())));
	};
	std::thread thr(addToDB, imgVec);
	thr.detach();

	return true;
}

bool MainWindow::loadFile(const QString& fileName, bool source) {
	std::filesystem::path currentPath(fileName.toStdString());
	if (lastpath)
		delete(lastpath);
	lastpath = new QString(currentPath.string().c_str());

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
	if (source) {
		img::Image image(QDir::cleanPath(fileName).toStdString(), cv::IMREAD_COLOR);
		lnkr::setSourceImage(image);
		if (source_img) {
			if (gen::cmpMat(image.getImageMat(), source_img->getImageMat()));
			else { 
				delete(source_img);
				source_img = nullptr;
				source_img = new img::Image(image);
				lnkr::deleteFromSimAndWV();
				resultModel->resetTable();
				delete(comparator);
				comparator = nullptr;
			}
		}
		else {
			source_img = new img::Image(image);
		}		
	}
	else if (!source) {
		if (dest_img) {
			delete(dest_img);
			dest_img = nullptr;
		}
		img::Image image(QDir::cleanPath(fileName).toStdString(), cv::IMREAD_COLOR);
		dest_img = new img::Image(lnkr::setDestinationImage(image));
	}

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

void MainWindow::initializeImageFileDialog(QFileDialog& dialog, QFileDialog::AcceptMode acceptMode, bool multipleSelection) {
	static bool firstDialog = true;
	if (firstDialog) {
		firstDialog = false;
		const QString picturesLocations = *lastpath;		
		dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations);
	}
	dialog.setFileMode(multipleSelection ? QFileDialog::ExistingFiles : QFileDialog::ExistingFile);
	//dialog.setOption(QFileDialog::Option::DontUseNativeDialog, true);

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

void MainWindow::openList() {
	QFileDialog dialog(this, tr("Open Files"));
	initializeImageFileDialog(dialog, QFileDialog::AcceptOpen, true);

	while (dialog.exec() == QDialog::Accepted && !loadFiles(dialog.selectedFiles(), imageCheck)) {}
}

void MainWindow::openImageLabel() {
	QFileDialog dialog(this, tr("Open File"));
	initializeImageFileDialog(dialog, QFileDialog::AcceptOpen, false);

	while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first())) {}
}

void MainWindow::addToMainTable(img::Image* image) {
	lnkr::addToMainTable(image);
	mainModel->insertImage(*image);
}

void MainWindow::addToMainTableNoRefresh(img::Image* image) {
	lnkr::addToMainTable(image);
	mainModel->insertImage(*image);
}

void MainWindow::createActions()
//! [17] //! [18]
{
	
}

void MainWindow::customMenuRequested_main(QPoint pos) {
	QModelIndex index_proxy = ui.mainTableView->indexAt(pos);
	auto index_main = proxyModel_main->mapToSource(index_proxy);
	auto hash = mainModel->data(index_main.siblingAtColumn(1));
	if (dir)
		delete(dir);
	dir = new QString(lnkr::getImageDir(hash.toString().toStdString()).c_str());

	QMenu* menu_main = new QMenu(this);
	QAction* action1 = new QAction("Pick as source image", menu_main);
	QAction* action2 = new QAction("Pick as comparison image", menu_main);
	QAction* action3 = new QAction("Delete from table(will also delete from database)", menu_main);
	QAction* action4 = new QAction("Copy full path to clipboard", menu_main);
	QAction* action5 = new QAction("Copy folder path to clipboard", menu_main);

	menu_main->addAction(action1);
	menu_main->addAction(action2);
	menu_main->addAction(action3);
	menu_main->addAction(action4);
	menu_main->addAction(action5);
	menu_main->popup(ui.mainTableView->viewport()->mapToGlobal(pos));

	QObject::connect(action1, &QAction::triggered, this, [this] { loadFile(*dir); });
	QObject::connect(action2, &QAction::triggered, this, [this] { loadFile(*dir, false); });
	QObject::connect(action3, &QAction::triggered, this, [this] { deleteImage(*dir); });
	QObject::connect(action4, &QAction::triggered, this, [this] { copyToClipboard(*dir); });
	QObject::connect(action5, &QAction::triggered, this, [this] { copyToClipboard(*dir, true); });
}

void MainWindow::customMenuRequested_result(QPoint pos) {
	QModelIndex index_proxy = ui.resultTableView->indexAt(pos);
	auto index_result = proxyModel_result->mapToSource(index_proxy);
	auto hash = resultModel->data(index_result.siblingAtColumn(1)).toString().toStdString();
	if (dir) 
		delete(dir);
	dir = new QString(lnkr::getImageDir(hash).c_str());

	if (currentcomp) {
		delete(currentcomp);
		currentcomp = nullptr;
	}
	currentcomp = new iop::Comparison(lnkr::getRawComparison(hash, false));
	currentcomp->source_dir = source_img->getVariablesString()[1];
	currentcomp->rhand_dir = dir->toStdString();

	QMenu* menu = new QMenu(this);
	QAction* action1 = new QAction("Pick as source image", menu);
	QAction* action2 = new QAction("Pick as comparison image", menu);
	QAction* action3 = new QAction("Delete from table(will also delete from database)", menu);
	QAction* action4 = new QAction("Copy full path to clipboard", menu);
	QAction* action5 = new QAction("Copy folder path to clipboard", menu);
	QAction* action6 = new QAction("View details...", menu);

	menu->addAction(action1);
	menu->addAction(action2);
	menu->addAction(action3);
	menu->addAction(action4);
	menu->addAction(action5);
	menu->addAction(action6);
	menu->popup(ui.mainTableView->viewport()->mapToGlobal(pos));

	QObject::connect(action1, &QAction::triggered, this, [&] { loadFile(*dir); });
	QObject::connect(action2, &QAction::triggered, this, [this] { loadFile(*dir, false); });
	QObject::connect(action3, &QAction::triggered, this, [this] { deleteImage(*dir); });
	QObject::connect(action4, &QAction::triggered, this, [this] { copyToClipboard(*dir); });
	QObject::connect(action5, &QAction::triggered, this, [this] { copyToClipboard(*dir, true); });
	QObject::connect(action6, &QAction::triggered, this, [&] { openDetailDialog(currentcomp, currentwv); });
}

void MainWindow::copyToClipboard(QString& str, bool folder) {
	QClipboard* clipboard = QApplication::clipboard();

	if (folder) {
		QString folderPath;
		string oper = str.toStdString();
		int j = 0;
		for (int i = 0; i < oper.size(); i++) {
			if (oper[i] == '/' || oper[i] == '\\') {
				j = i;
			}
		}
		folderPath = oper.substr(0, j + 1).c_str();
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
	int i = 0;
	for (i = 0; i < mainModel->rowCount(); i++)
		if (mainModel->data(mainModel->index(i, 0), 0).toString().toStdString() == image.getVariablesString()[1])
			break;
	mainModel->removeRow(i);
}

void MainWindow::refreshTable(TableModel* table) {
	if (table->getType() == table_type::TABLE_MAIN) {
		table->removeRows(0, table->rowCount());
		auto imageVec = lnkr::getImageTable();
		for (auto i : imageVec)
			table->insertImage(i);
	}

	else if (table->getType() == table_type::TABLE_RESULT) {
		table->removeRows(0, table->rowCount());
		auto compVec = lnkr::getSimilarityTable();
		for (auto i : compVec)
			table->insertComparison(i);
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

void MainWindow::compareMain() {
	if (!currentfv && !currentwv) {
		openSortDialog();
		return;
	}
	else if (!currentfv && currentwv || currentfv && !currentwv) {
		throw gen::Warning("One of feature vector or weight vector can't be null. They must both have the same state.");
		return;
	}

	lnkr::deleteFromSimAndWV();

	if (!source_img) {
		throw gen::Warning("Source image must be picked first.");
		return;
	}

	auto fv = *currentfv;
	auto* source_image = new img::Image(*source_img);
	auto recommended = 200;
	auto source_mat = source_image->getImageMat();

	auto fv_edge = *fv.edge;
	feat::Edge* source_edge = nullptr;

	if (!fv.edge->empty()) {
		auto height = source_mat.rows;
		auto width = source_mat.cols;
		if (height > width) {
			auto temp = recommended;
			width = recommended * width / height;
			height = temp;
		}
		else {
			auto temp = recommended;
			height = recommended * height / width;
			width = temp;
		}
		source_edge = new feat::Edge(source_mat, fv_edge.getEdgeFlag(), fv_edge.getCannyPtr(),
			width, fv_edge.getComparisonValues()[1], fv_edge.getComparisonValues()[2]);
	}

	auto fv_hgray = *fv.hist_gray;
	feat::Histogram* source_hgray = nullptr;
	if (!fv_hgray.empty()) {
		source_hgray = new feat::Histogram(source_mat, static_cast<int>(fv_hgray.getVariablesFloat()[0]),
			static_cast<int>(fv_hgray.getVariablesFloat()[1]), static_cast<int>(fv_hgray.getVariablesFloat()[2]),
			static_cast<int>(fv_hgray.getVariablesFloat()[3]));
	}

	auto fv_hbgr = *fv.hist_bgr;
	feat::Histogram* source_hbgr = nullptr;
	if (!fv_hbgr.empty()) {
		source_hbgr = new feat::Histogram(source_mat, static_cast<int>(fv_hbgr.getVariablesFloat()[0]),
			static_cast<int>(fv_hbgr.getVariablesFloat()[1]), static_cast<int>(fv_hbgr.getVariablesFloat()[2]),
			static_cast<int>(fv_hbgr.getVariablesFloat()[3]));
	}

	auto fv_hhsv = *fv.hist_hsv;
	feat::Histogram* source_hhsv = nullptr;
	if (!fv_hhsv.empty()) {
		source_hhsv = new feat::Histogram(source_mat, static_cast<int>(fv_hhsv.getVariablesFloat()[0]),
			static_cast<int>(fv_hhsv.getVariablesFloat()[1]), static_cast<int>(fv_hhsv.getVariablesFloat()[2]),
			static_cast<int>(fv_hhsv.getVariablesFloat()[3]));
	}

	auto fv_hash = *fv.perc_hash;
	feat::Hash* source_hash = nullptr;
	if (!fv_hash.empty()) {
		source_hash = new feat::Hash(source_mat, fv_hash.getSelectHash());
	}

	auto fv_source = new iop::FeatureVector(source_image, source_edge, source_hgray, source_hbgr, 
		source_hhsv, source_hash);

	auto wv = *currentwv;

	auto imageVec = lnkr::getImageDirs();
	if (!comparator)
		delete(comparator);

	comparator = new iop::Comparator();

	comparator->beginMultiCompare(fv_source, imageVec, &wv, CMP_MAX);

	comparisonOperations(*comparator);

	auto addToDB = [this](std::vector<iop::Comparison> compVec) {
		auto* main_ptr = mainModel->getImgVecPtr();
		for (int i = 0; i < main_ptr->size(); i++) {
			resultModel->insertImage(main_ptr->at(i).first);
		}
		auto addToTable = [&](iop::Comparison comp) {
			bool check = false;
			int i = 0;
			for (i = 0; i < main_ptr->size(); i++)
				if (main_ptr->at(i).first.getVariablesString()[1] == comp.rhand_dir) {
					check = true;
					break;
				}
			if (check)
				resultModel->updateComparison(i, comp);
		};
		int rc = sqlite3_exec(mw_dbPtr->databasePtr(), "BEGIN TRANSACTION;", NULL, NULL, NULL);
		mw_dbPtr->errorCheck(rc, const_cast<char*>(sqlite3_errmsg(mw_dbPtr->databasePtr())));
		int j = 1;
		for (auto &i : compVec) {
			lnkr::setSimilarity(&i);
			std::thread tablethread(addToTable, i);
			tablethread.detach();
			j++;
		}
		rc = sqlite3_exec(mw_dbPtr->databasePtr(), "END TRANSACTION;", NULL, NULL, NULL);
		mw_dbPtr->errorCheck(rc, const_cast<char*>(sqlite3_errmsg(mw_dbPtr->databasePtr())));
	};
	//SIMILARITY TABLOSUNU HALLEDERSIN YARIN ÞÝPÞAK DEMÝ ASLAN PARÇASI ASLAAAN
	resultModel->resetTable();
	lnkr::setWeightVector(&wv);
	std::thread dbthread(addToDB, comparator->getComparisonVector(false));
	dbthread.detach();
	ui.stackedWidget_table->setCurrentIndex(1);

	delete(fv_source);
}

void MainWindow::switchTables() {
	if (ui.stackedWidget_table->currentIndex() == 0) {
		int pos_res = ui.resultTableView->indexAt(ui.resultTableView->rect().topLeft()).row();
		proxyModel_result->suspendData({ pos_res - 5, pos_res + 15 });
		ui.stackedWidget_table->setCurrentIndex(1);
	}
	else if (ui.stackedWidget_table->currentIndex() == 1) {
		int pos_main = ui.mainTableView->indexAt(ui.mainTableView->rect().topLeft()).row();
		proxyModel_main->suspendData({ pos_main - 5, pos_main + 15 });
		ui.stackedWidget_table->setCurrentIndex(0);
	}
}

void MainWindow::resizeEvent(QResizeEvent* event) {
	QMainWindow::resizeEvent(event);
	screenSize->setWidth(this->width());
	screenSize->setHeight(this->height());
	auto loadMain = [&]() {
		if (source_img)
			setImage(ui.label_imgSrc, cvMatToQImage(source_img->getImageMat()));
		if (dest_img)
			setImage(ui.label_imgDest, cvMatToQImage(dest_img->getImageMat()));
	};

	auto loadComp = [&]() {
		displayFeature(false, curr_feat_src.index);
	};
	if (ui.mainTabWidget->currentIndex() == 0) {
		loadMain();
		if(curr_feat_src.index != -1) {
			ui.mainTabWidget->setCurrentIndex(1);
			loadComp();
			ui.mainTabWidget->setCurrentIndex(0);
		}
	}
	else if (ui.mainTabWidget->currentIndex() == 1) {
		if (curr_feat_src.index != -1) {
			loadComp();
		}
		ui.mainTabWidget->setCurrentIndex(0);
		loadMain();
		ui.mainTabWidget->setCurrentIndex(1);
		
	}
}

void currentFeature::destroyCurrent() {
	int index = this->index;
	if (index == -1)
		return;
	else if (index == 0)
		return;
	else if (index == 1)
		return;
	else if (index == 2)
		this->feature.edge.~Edge();
	else if (index == 3)
		this->feature.corner.~Corner();
}

void MainWindow::comparisonOperations(iop::Comparator& comparator) {
	auto normalizeFeature = [](std::vector<float*> featVec) {
		std::vector<float> featVals;
		for (int i = 0; i < featVec.size(); i++) {
			featVals.push_back(*featVec[i]);
		}

		cv::Mat normOper(cv::Size(featVals.size(), 1), CV_32FC1, featVals.data());
		cv::normalize(normOper, normOper, 1, 0, cv::NORM_MINMAX);

		featVals.clear();
		if (normOper.isContinuous()) {
			featVals.assign((float*)normOper.data, (float*)normOper.data + normOper.total() * normOper.channels());
		}
		else {
			for (int i = 0; i < normOper.rows; ++i) {
				featVals.insert(featVals.end(), normOper.ptr<float>(i), normOper.ptr<float>(i) + normOper.cols * normOper.channels());
			}
		}

		for (int i = 0; i < featVals.size(); i++) {
			*featVec[i] = featVals[i];
		}
	};

	auto& compVec = *comparator.getComparisonVector_ptr();
	if (compVec.size() == 0)
		return;
	for (int i = 0; i < compVec.at(0).diff_total.size(); i++) {
		std::vector<float*> featVec;
		for (int j = 0; j < compVec.size(); j++) {
			if (*compVec.at(j).diff_total[i] != -1)
				featVec.push_back(compVec.at(j).diff_total[i]);
		}
		if (featVec.size() != 0)
			normalizeFeature(featVec);
	}

	for (int i = 0; i < compVec.size(); i++) {
		compVec[i].wvec = new iop::WeightVector(*comparator.getWeightVector());
		compVec[i].calculateEuclideanDistance();
		delete(compVec[i].wvec);
	}
}

void MainWindow::statisticsOperations() {
}

void MainWindow::statsTab_enableBGR_src(int index) {
	if (index == 3) {
		ui.stackedWidget->setHidden(false);
		ui.stackedWidget->setCurrentIndex(0);
	}
	else if (index == 4) {
		ui.stackedWidget->setHidden(false);
		ui.stackedWidget->setCurrentIndex(1);
	}
	else {
		ui.stackedWidget->setHidden(true);
	}
}

void MainWindow::statsTab_enableBGR_dest(int index) {
	if (index == 3) {
		ui.stackedWidget_2->setHidden(false);
		ui.stackedWidget_2->setCurrentIndex(0);
	}
	else if (index == 4) {
		ui.stackedWidget_2->setHidden(false);
		ui.stackedWidget_2->setCurrentIndex(1);
	}
	else {
		ui.stackedWidget_2->setHidden(true);
	}
}

void MainWindow::displayStatistics() {
	int src_index = ui.comboBox_statSrc->currentIndex();
	int dest_index = ui.comboBox_statDest->currentIndex();

	auto displayHist = [&](int index, bool source, bool bgr) {
		cv::Mat1f histMat;
		auto compVec = comparator->getComparisonVector(false);
		if (bgr) {
			if (index == 0) {
				for (int i = 0; i < compVec.size(); i++)
					histMat.push_back(compVec[i].diff_hbgrb);
				feat::Histogram hist(histMat, HIST_DATA, 100);
				displayHistogram(nullptr, source, &hist);
			}
			else if (index == 1) {
				for (int i = 0; i < compVec.size(); i++)
					histMat.push_back(compVec[i].diff_hbgrg);
				feat::Histogram hist(histMat, HIST_DATA, 100);
				displayHistogram(nullptr, source, &hist);
			}
			else if (index == 2) {
				for (int i = 0; i < compVec.size(); i++)
					histMat.push_back(compVec[i].diff_hbgrr);
				feat::Histogram hist(histMat, HIST_DATA, 100);
				displayHistogram(nullptr, source, &hist);
			}
		}
		else {
			if (index == 0) {
				for (int i = 0; i < compVec.size(); i++)
					histMat.push_back(compVec[i].diff_hhsvh);
				feat::Histogram hist(histMat, HIST_DATA, 100);
				displayHistogram(nullptr, source, &hist);
			}
			else if (index == 1) {
				for (int i = 0; i < compVec.size(); i++)
					histMat.push_back(compVec[i].diff_hhsvs);
				feat::Histogram hist(histMat, HIST_DATA, 100);
				displayHistogram(nullptr, source, &hist);
			}
			else if (index == 2) {
				for (int i = 0; i < compVec.size(); i++)
					histMat.push_back(compVec[i].diff_hhsvv);
				feat::Histogram hist(histMat, HIST_DATA, 100);
				displayHistogram(nullptr, source, &hist);
			}
		}
	};

	auto display = [&](int combobox_index, bool source) {
		cv::Mat1f histMat;
		auto compVec = comparator->getComparisonVector(false);
		switch (combobox_index) {
		case 0: {
			for (int i = 0; i < compVec.size(); i++)
				histMat.push_back(compVec[i].diff_gradm);
			feat::Histogram hist(histMat, HIST_DATA, 100);
			displayHistogram(nullptr, source, &hist);
			break;
		}
		case 1: {
			for (int i = 0; i < compVec.size(); i++)
				histMat.push_back(compVec[i].diff_gradd);
			feat::Histogram hist(histMat, HIST_DATA, 100);
			displayHistogram(nullptr, source, &hist);
			break;
		}
		case 2: {
			for (int i = 0; i < compVec.size(); i++)
				histMat.push_back(compVec[i].diff_hgray);
			feat::Histogram hist(histMat, HIST_DATA, 100);
			displayHistogram(nullptr, source, &hist);
			break;
		}
		case 3: {
			if (source) {
				if (ui.radioButton_lb->isChecked())
					displayHist(0, true, true);
				else if (ui.radioButton_lg->isChecked())
					displayHist(1, true, true);
				else if (ui.radioButton_lr->isChecked())
					displayHist(2, true, true);
			}
			else {
				if (ui.radioButton_rb->isChecked())
					displayHist(0, false, true);
				else if (ui.radioButton_rg->isChecked())
					displayHist(1, false, true);
				else if (ui.radioButton_rr->isChecked())
					displayHist(2, false, true);
			}
			break;
		}
		case 4: {
			if (source) {
				if (ui.radioButton_lh->isChecked())
					displayHist(0, true, false);
				else if (ui.radioButton_ls->isChecked())
					displayHist(1, true, false);
				else if (ui.radioButton_lv->isChecked())
					displayHist(2, true, false);
			}
			else {
				if (ui.radioButton_rh->isChecked())
					displayHist(0, false, false);
				else if (ui.radioButton_rs->isChecked())
					displayHist(1, false, false);
				else if (ui.radioButton_rv->isChecked())
					displayHist(2, false, false);
			}
			break;
		}
		case 5: {
			for (int i = 0; i < compVec.size(); i++)
				histMat.push_back(compVec[i].diff_hashp);
			feat::Histogram hist(histMat, HIST_DATA, 100);
			displayHistogram(nullptr, source, &hist);
			break;
		}
		case 6: {
			for (int i = 0; i < compVec.size(); i++)
				histMat.push_back(compVec[i].diff_hashd);
			feat::Histogram hist(histMat, HIST_DATA, 100);
			displayHistogram(nullptr, source, &hist);
			break;
		}
		case 7: {
			for (int i = 0; i < compVec.size(); i++)
				histMat.push_back(compVec[i].euc_dist);
			feat::Histogram hist(histMat, HIST_DATA, 100);
			displayHistogram(nullptr, source, &hist);
			break;
		}
		default:
			throw std::exception("Illegal table index.");
			break;
		}
	};
	if (!comparator)
		throw std::exception("You need to do comparison operation first to see it's statistics.");
	display(src_index, true);
	display(dest_index, false);
}