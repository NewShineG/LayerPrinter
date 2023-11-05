#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"workThread.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
	, settings(SETTINGSPATH, QSettings::IniFormat)
{
    ui->setupUi(this);
	window_started();
	PosTimer = new QTimer(this);
	
	//dmdUSB连接初始化
	USB_Init();

	/*-------连接事件-------*/
	connect(ui->DMDConnectButton, &QPushButton::clicked, this, [this]() {
		this->DMDConnect();
	});
	connect(ui->AddPatternsButton, &QPushButton::clicked, this, [this]() {
		this->addPatterns();
	});
	connect(ui->ClearPatternsButton, &QPushButton::clicked, this, [this]() {
		this->clearPatterns();
	});
	connect(ui->DebugButton, &QPushButton::clicked, this, [this]() {
		this->on_DebugButton_clicked();
	});
	connect(ui->UpdateLUTButton, &QPushButton::clicked, this, [this]() {
		this->updateLUT();
	});
	connect(ui->startPatSequenceButton, &QPushButton::clicked, this, [this]() {
		this->startPatSequence();
	});
	connect(ui->pausePatSequenceButton, &QPushButton::clicked, this, [this]() {
		this->pausePatSequence();
	});	
	connect(ui->stopPatSequenceButton, &QPushButton::clicked, this, [this]() {
		this->stopPatSequence();
	});

	connect(ui->expoTimeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this]() {
		expoTimeSpinBox_changed();
	});
	connect(ui->expoTimeUnitComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this]() {
		this->expoTimeUnitComboBox_changed();
	});
	connect(ui->darkTimeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this]() {
		darkTimeSpinBox_changed();
	});
	connect(ui->darkTimeUnitComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this]() {
		this->darkTimeUnitComboBox_changed();
	});
	connect(ui->DMDRepeatCheckBox, &QCheckBox::clicked, this, [this]() {
		this->DMDRepeatCheckBox_changed();
	});
	connect(ui->DMDPlayOnceCheckBox, &QCheckBox::clicked, this, [this]() {
		this->DMDPlayOnceCheckBox_changed();
	});
	connect(ui->DMDTrigInCheckBox, &QCheckBox::clicked, this, [this]() {
		this->DMDTrigInCheckBox_changed();
	});
	connect(ui->DMDTrigOutCheckBox, &QCheckBox::clicked, this, [this]() {
		this->DMDTrigOutCheckBox_changed();
	});
	connect(ui->zStepDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this]() {
		zStep = ui->zStepDoubleSpinBox->value() * zStepUnit;
	});
	connect(ui->zStepUnitComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this]() {
		this->zStepUnitComboBox_changed();
	});
	connect(ui->zTargetDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this]() {
		zTarget = ui->zTargetDoubleSpinBox->value();
	});
	connect(ui->zVelocityDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this]() {
		zVelocity = ui->zVelocityDoubleSpinBox->value();
	});
	connect(ui->xStepDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this]() {
		xStep = ui->xStepDoubleSpinBox->value() * xStepUnit;
	});
	connect(ui->xStepUnitComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this]() {
		this->xStepUnitComboBox_changed();
	});
	connect(ui->xTargetDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this]() {
		xTarget = ui->xTargetDoubleSpinBox->value();
	});
	connect(ui->yStepDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this]() {
		yStep = ui->yStepDoubleSpinBox->value() * yStepUnit;
	});
	connect(ui->yStepUnitComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this]() {
		this->yStepUnitComboBox_changed();
	});
	connect(ui->yTargetDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this]() {
		yTarget = ui->yTargetDoubleSpinBox->value();
	});
	connect(ui->savePatternConfigurationButton, &QPushButton::clicked, this, [this]() {
		this->savePatternConfiguration();
	});
	connect(ui->loadPatternConfigurationButton, &QPushButton::clicked, this, [this]() {
		this->loadPatternConfiguration();
	});

	//Stage
	connect(ui->stageConnectButton, &QPushButton::clicked, this, [this]() {
		this->stageConnect();
	});
	connect(ui->zAddButton, &QPushButton::clicked, this, [this]() {
		this->zAdd();
	});
	connect(ui->zMinusButton, &QPushButton::clicked, this, [this]() {
		this->zMinus();
	});
	connect(ui->zTargetMoveButton, &QPushButton::clicked, this, [this]() {
		this->zTargetMove();
	});
	connect(ui->zTargetDoubleSpinBox, &QDoubleSpinBox::editingFinished, this, [this]() {
		this->zTargetMove();
	});
	connect(ui->zVelocityMoveButton, &QPushButton::clicked, this, [this]() {
		this->zVelocityMove();
	});
	connect(ui->zStopButton, &QPushButton::clicked, this, [this]() {
		this->zStop();
	});

	connect(ui->xAddButton, &QPushButton::clicked, this, [this]() {
		this->xAdd();
	});
	connect(ui->xMinusButton, &QPushButton::clicked, this, [this]() {
		this->xMinus();
	});
	connect(ui->xTargetMoveButton, &QPushButton::clicked, this, [this]() {
		this->xTargetMove();
	});
	connect(ui->xTargetDoubleSpinBox, &QDoubleSpinBox::editingFinished, this, [this]() {
		this->xTargetMove();
	});

	connect(ui->yAddButton, &QPushButton::clicked, this, [this]() {
		this->yAdd();
	});
	connect(ui->yMinusButton, &QPushButton::clicked, this, [this]() {
		this->yMinus();
	});
	connect(ui->yTargetMoveButton, &QPushButton::clicked, this, [this]() {
		this->yTargetMove();
	});
	connect(ui->yTargetDoubleSpinBox, &QDoubleSpinBox::editingFinished, this, [this]() {
		this->yTargetMove();
	});

	connect(ui->StepMoveCheckBox, &QCheckBox::clicked, this, [this]() {
		ui->VelocityMoveCheckBox->setChecked(!ui->StepMoveCheckBox->isChecked());
		isStepMove = ui->StepMoveCheckBox->isChecked();
	});
	connect(ui->VelocityMoveCheckBox, &QCheckBox::clicked, this, [this]() {
		ui->StepMoveCheckBox->setChecked(!ui->VelocityMoveCheckBox->isChecked());
		isStepMove = ui->StepMoveCheckBox->isChecked();
	});
	connect(ui->LayerHeightDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this]() {
		layerHeight = ui->LayerHeightDoubleSpinBox->value();
		structHeight = layerHeight * layerNum;
		ui->StructHeightDoubleSpinBox->setValue(structHeight);
	});
	//connect(ui->StructHeightDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this]() {
	//	structHeight = ui->StructHeightDoubleSpinBox->value();
	//	layerNum = (int)(structHeight / layerHeight);
	//	ui->LayerNumberSpinBox->setValue(layerNum);
	//	structHeight = layerHeight * layerNum;
	//	ui->StructHeightDoubleSpinBox->setValue(structHeight);
	//});
	connect(ui->LayerNumberSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this]() {
		layerNum = ui->LayerNumberSpinBox->value();
		structHeight = layerHeight * layerNum;
		ui->StructHeightDoubleSpinBox->setValue(structHeight);
	});
	connect(ui->startLayerPrintButton, &QPushButton::clicked, this, [this]() {
		this->startLayerPrint();
	});
	connect(ui->stopLayerPrintButton, &QPushButton::clicked, this, [this]() {
		this->stopLayerPrint();
	});

	connect(PosTimer, &QTimer::timeout, this, &MainWindow::updatePos);
	PosTimer->start(100);
	
}

MainWindow::~MainWindow()
{
	USB_Close();
	PI_CloseConnection(ID0);
	settings.setValue("m_ptnImagePath", this->m_ptnImagePath);
	settings.setValue("expoTime", this->expoTime);
	settings.setValue("darkTime", this->darkTime);
	settings.setValue("expoTimeUnit", this->expoTimeUnit);
	settings.setValue("darkTimeUnit", this->darkTimeUnit);
	settings.setValue("zStep", this->zStep);
	settings.setValue("xStep", this->xStep);
	settings.setValue("yStep", this->yStep);
	settings.setValue("zStepUnit", this->zStepUnit);
	settings.setValue("xStepUnit", this->xStepUnit);
	settings.setValue("yStepUnit", this->yStepUnit);
	settings.setValue("isRepeat", this->isRepeat);
	settings.setValue("isTrigIn", this->isTrigIn);
	settings.setValue("isTrigOut", this->isTrigOut);
	settings.setValue("layerHeight", this->layerHeight);
	settings.sync();

	delete PosTimer;
    delete ui;
}




void MainWindow::showError(const char *str)
{
	ui->ErrorLabel->setText(str);
}


/**
f * @brief MainWindow::getStatus
 * reads various status indicators from the controller and updates the Status checkboxes accordingly
 */
void MainWindow::getStatus()
{
	unsigned char HWStatus, SysStatus, MainStatus;
	BootLoaderStaus BLStatus;

	if (LCR_GetStatus(&HWStatus, &SysStatus, &MainStatus) == 0)
	{
		if (SysStatus & BIT0)
			ui->internalMemTest_checkBox->setChecked(true);
		else
			ui->internalMemTest_checkBox->setChecked(false);

		if (HWStatus & BIT0)
			ui->internalInit_checkBox->setChecked(true);
		else
			ui->internalInit_checkBox->setChecked(false);

		if (HWStatus & BIT1)
			ui->incompatibleASICorDMD_checkBox->setChecked(true);
		else
			ui->incompatibleASICorDMD_checkBox->setChecked(false);

		if (HWStatus & BIT4)
			ui->slaveReady_checkBox->setChecked(true);
		else
			ui->slaveReady_checkBox->setChecked(false);

		if (HWStatus & BIT2)
			ui->DMDResetWaveformControllerErr_checkBox->setChecked(true);
		else
			ui->DMDResetWaveformControllerErr_checkBox->setChecked(false);

		if (HWStatus & BIT3)
			ui->forcedSwapErr_checkBox->setChecked(true);
		else
			ui->forcedSwapErr_checkBox->setChecked(false);

		if (HWStatus & BIT6)
			ui->seqAbortStatus_checkBox->setChecked(true);
		else
			ui->seqAbortStatus_checkBox->setChecked(false);

		if (HWStatus & BIT7)
			ui->seqErr_checkBox->setChecked(true);
		else
			ui->seqErr_checkBox->setChecked(false);

		if (MainStatus & BIT0)
			ui->DMDParkStatus_checkBox->setChecked(true);
		else
			ui->DMDParkStatus_checkBox->setChecked(false);

		if (MainStatus & BIT1)
			ui->sequencerRunState_checkBox->setChecked(true);
		else
			ui->sequencerRunState_checkBox->setChecked(false);

		if (MainStatus & BIT2)
			ui->videoState_checkBox->setChecked(false);
		else
			ui->videoState_checkBox->setChecked(true);

		if (MainStatus & BIT3)
			ui->extSourceLocked_checkBox->setChecked(true);
		else
			ui->extSourceLocked_checkBox->setChecked(false);
	}
	else if (LCR_GetBLStatus(&BLStatus) == 0)
	{
		//This means the device is in boot mode
	}
	else
	{
		USB_Close();
	}

}



//界面初始化
void MainWindow::window_started()
{
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	if (settings.contains("m_ptnImagePath"))
	{
		this->m_ptnImagePath = settings.value("m_ptnImagePath").toString();
	}
	//DMD连接状态控件
	ui->DMDConnectCheckBox->setChecked(0);
	ui->DMDConnectCheckBox->setText("未连接");

	//曝光时间控件
	ui->expoTimeUnitComboBox->addItem("us");
	ui->expoTimeUnitComboBox->addItem("ms");
	ui->expoTimeUnitComboBox->addItem("s");
	if (settings.contains("expoTimeUnit"))
	{
		this->expoTimeUnit = settings.value("expoTimeUnit").toInt();
		switch (expoTimeUnit) {
		case 1:				ui->expoTimeUnitComboBox->setCurrentText("us"); break;
		case 1000:		ui->expoTimeUnitComboBox->setCurrentText("ms"); break;
		case 1000000:	ui->expoTimeUnitComboBox->setCurrentText("s"); break;
		default:
		{
			ui->expoTimeUnitComboBox->setCurrentText("N/A");
			expoTimeUnit = 1;
			break;
		}
		}
	}

	ui->expoTimeSpinBox->setRange(0, 1000000000);
	ui->expoTimeSpinBox->setSingleStep(1);
	if (settings.contains("expoTime"))
	{
		this->expoTime = settings.value("expoTime").toInt();
		ui->expoTimeSpinBox->setValue(expoTime / expoTimeUnit);
	}

	//暗光时间控件
	ui->darkTimeUnitComboBox->addItem("us");
	ui->darkTimeUnitComboBox->addItem("ms");
	ui->darkTimeUnitComboBox->addItem("s");
	if (settings.contains("darkTimeUnit"))
	{
		this->darkTimeUnit = settings.value("darkTimeUnit").toInt();
		switch (darkTimeUnit) {
		case 1:				ui->darkTimeUnitComboBox->setCurrentText("us"); break;
		case 1000:		ui->darkTimeUnitComboBox->setCurrentText("ms"); break;
		case 1000000:	ui->darkTimeUnitComboBox->setCurrentText("s"); break;
		default:
		{
			ui->darkTimeUnitComboBox->setCurrentText("N/A");
			darkTimeUnit = 1;
			break;
		}
		}
	}

	ui->darkTimeSpinBox->setRange(0, 1000000000);
	ui->darkTimeSpinBox->setSingleStep(1);
	if (settings.contains("darkTime"))
	{
		this->darkTime = settings.value("darkTime").toInt();
		ui->darkTimeSpinBox->setValue(darkTime / darkTimeUnit);
	}

	//重复or一次 控件
	if (settings.contains("isRepeat"))
	{
		this->isRepeat = settings.value("isRepeat").toBool();
		if (isRepeat)
		{
			ui->DMDRepeatCheckBox->setChecked(1);
			ui->DMDPlayOnceCheckBox->setChecked(0);
		}
		else {
			ui->DMDRepeatCheckBox->setChecked(0);
			ui->DMDPlayOnceCheckBox->setChecked(1);
		}
	}
	else
	{
		ui->DMDRepeatCheckBox->setChecked(0);
		ui->DMDPlayOnceCheckBox->setChecked(1);
	}

	//TriggerIn and TriggerOut 控件
	if (settings.contains("isTrigerIn"))
	{
		this->isTrigIn = settings.value("isTrigIn").toBool();
		if (isTrigIn)	ui->DMDTrigInCheckBox->setChecked(1);
		else					ui->DMDTrigInCheckBox->setChecked(0);
	}
	if (settings.contains("isTrigOut"))
	{
		this->isTrigOut = settings.value("isTrigOut").toBool();
		if (isTrigOut)	ui->DMDTrigOutCheckBox->setChecked(1);
		else					ui->DMDTrigOutCheckBox->setChecked(0);
	}


	//位移台连接状态控件
	ui->stageConnectCheckBox->setChecked(0);
	ui->stageConnectCheckBox->setText("未连接");

	//Z轴步长及位置控件
	ui->zStepUnitComboBox->addItem("nm");
	ui->zStepUnitComboBox->addItem("um");
	ui->zStepUnitComboBox->addItem("mm");
	if (settings.contains("zStepUnit"))
	{
		this->zStepUnit = settings.value("zStepUnit").toDouble();
		if(zStepUnit == 0.001)
			ui->zStepUnitComboBox->setCurrentText("nm");
		else if (zStepUnit == 1)
			ui->zStepUnitComboBox->setCurrentText("um");
		else if (zStepUnit == 1000)
			ui->zStepUnitComboBox->setCurrentText("mm");
		else
		{
			zStepUnit = 1;
			ui->zStepUnitComboBox->setCurrentText("um");
		}
	}

	ui->zStepDoubleSpinBox->setRange(0, 1000000000);
	ui->zStepDoubleSpinBox->setSingleStep(1);
	if (settings.contains("zStep"))
	{
		this->zStep = settings.value("zStep").toDouble();
		ui->zStepDoubleSpinBox->setValue(zStep / zStepUnit);
	}

	ui->zTargetDoubleSpinBox->setDecimals(3);
	ui->zPosLabel->setText("N/A");

	//X轴步长及位置控件
	ui->xStepUnitComboBox->addItem("nm");
	ui->xStepUnitComboBox->addItem("um");
	ui->xStepUnitComboBox->addItem("mm");
	if (settings.contains("xStepUnit"))
	{
		this->xStepUnit = settings.value("xStepUnit").toDouble();
		if (xStepUnit == 0.001)
			ui->xStepUnitComboBox->setCurrentText("nm");
		else if (xStepUnit == 1)
			ui->xStepUnitComboBox->setCurrentText("um");
		else if (xStepUnit == 1000)
			ui->xStepUnitComboBox->setCurrentText("mm");
		else
		{
			xStepUnit = 1;
			ui->xStepUnitComboBox->setCurrentText("um");
		}
	}

	ui->xStepDoubleSpinBox->setRange(0, 1000000000);
	ui->xStepDoubleSpinBox->setSingleStep(1);
	if (settings.contains("xStep"))
	{
		this->xStep = settings.value("xStep").toDouble();
		ui->xStepDoubleSpinBox->setValue(xStep / xStepUnit);
	}

	ui->xTargetDoubleSpinBox->setDecimals(3);
	ui->xPosLabel->setText("N/A");

	//Y轴步长及位置控件
	ui->yStepUnitComboBox->addItem("nm");
	ui->yStepUnitComboBox->addItem("um");
	ui->yStepUnitComboBox->addItem("mm");
	if (settings.contains("yStepUnit"))
	{
		this->yStepUnit = settings.value("yStepUnit").toDouble();
		if (yStepUnit == 0.001)
			ui->yStepUnitComboBox->setCurrentText("nm");
		else if (yStepUnit == 1)
			ui->yStepUnitComboBox->setCurrentText("um");
		else if (yStepUnit == 1000)
			ui->yStepUnitComboBox->setCurrentText("mm");
		else
		{
			yStepUnit = 1;
			ui->yStepUnitComboBox->setCurrentText("um");
		}
	}

	ui->yStepDoubleSpinBox->setRange(0, 1000000000);
	ui->yStepDoubleSpinBox->setSingleStep(1);
	if (settings.contains("yStep"))
	{
		this->yStep = settings.value("yStep").toDouble();
		ui->yStepDoubleSpinBox->setValue(yStep / yStepUnit);
	}

	ui->yTargetDoubleSpinBox->setDecimals(3);
	ui->yPosLabel->setText("N/A");

	if (settings.contains("layerHeight"))
	{
		this->layerHeight = settings.value("layerHeight").toDouble();
		ui->LayerHeightDoubleSpinBox->setValue(layerHeight);
	}
	
	ui->status_groupBox->setVisible(0);
	ui->StepMoveCheckBox->setChecked(1);
	//ui->VelocityMoveCheckBox->setEnabled(0);
	ui->zVelocityDoubleSpinBox->setEnabled(0);
	ui->zVelocityMoveButton->setEnabled(0);
	ui->StructHeightDoubleSpinBox->setEnabled(0);
	ui->LayerNumberSpinBox->setEnabled(0);
	ui->stopLayerPrintButton->setEnabled(0);
}


void MainWindow::expoTimeUnitComboBox_changed()
{
	QString unit = ui->expoTimeUnitComboBox->currentText();
	if (unit.compare("us") == 0) {
		expoTimeUnit = 1;
	}
	else if (unit.compare("ms") == 0) {
		expoTimeUnit = 1000;
	}
	else if (unit.compare("s") == 0) {
		expoTimeUnit = 1000000;
	}
	expoTime = ui->expoTimeSpinBox->value() * expoTimeUnit;
	int i;
	if (m_elements.size() > 0)
	{
		for (i = 0; i < m_elements.size(); i++)
			m_elements[i].exposure = expoTime;
	}
}

void MainWindow::expoTimeSpinBox_changed()
{
	expoTime = ui->expoTimeSpinBox->value() * expoTimeUnit;
	int i;
	if (m_elements.size() > 0)
	{
		for (i = 0; i < m_elements.size(); i++)
			m_elements[i].exposure = expoTime;
	}
}


void MainWindow::darkTimeUnitComboBox_changed()
{
	QString unit = ui->darkTimeUnitComboBox->currentText();
	if (unit.compare("us") == 0) {
		darkTimeUnit = 1;
	}
	else if (unit.compare("ms") == 0) {
		darkTimeUnit = 1000;
	}
	else if (unit.compare("s") == 0) {
		darkTimeUnit = 1000000;
	}
	darkTime = ui->darkTimeSpinBox->value() * darkTimeUnit;
	int i;
	if (m_elements.size() > 0)
	{
		for (i = 0; i < m_elements.size(); i++)
			m_elements[i].darkPeriod = darkTime;
	}
}

void MainWindow::darkTimeSpinBox_changed()
{
	darkTime = ui->darkTimeSpinBox->value() * darkTimeUnit;
	int i;
	if (m_elements.size() > 0)
	{
		for (i = 0; i < m_elements.size(); i++)
			m_elements[i].darkPeriod = darkTime;
	}
}

void MainWindow::DMDRepeatCheckBox_changed()
{
	isRepeat = ui->DMDRepeatCheckBox->isChecked();
	if (isRepeat)
	{
		ui->DMDRepeatCheckBox->setChecked(1);
		ui->DMDPlayOnceCheckBox->setChecked(0);
	}
	else {
		ui->DMDRepeatCheckBox->setChecked(0);
		ui->DMDPlayOnceCheckBox->setChecked(1);
	}
}

void MainWindow::DMDPlayOnceCheckBox_changed()
{
	isRepeat = ! ui->DMDPlayOnceCheckBox->isChecked();
	if (isRepeat)
	{
		ui->DMDRepeatCheckBox->setChecked(1);
		ui->DMDPlayOnceCheckBox->setChecked(0);
	}
	else {
		ui->DMDRepeatCheckBox->setChecked(0);
		ui->DMDPlayOnceCheckBox->setChecked(1);
	}
}

void MainWindow::DMDTrigInCheckBox_changed()
{
	int i;
	isTrigIn = ui->DMDTrigInCheckBox->isChecked();
	if (m_elements.size() > 0)
	{
		for (i = 0; i < m_elements.size(); i++)
			m_elements[i].trigIn = isTrigIn;
		update_listWidget();
	}
}

void MainWindow::DMDTrigOutCheckBox_changed()
{
	int i;
	isTrigOut = ui->DMDTrigOutCheckBox->isChecked();
	if (m_elements.size() > 0)
	{
		for (i = 0; i < m_elements.size(); i++)
			m_elements[i].trigOut2 = isTrigOut;
		
	}
}


void MainWindow::zStepUnitComboBox_changed()
{
	QString unit = ui->zStepUnitComboBox->currentText();
	if (unit.compare("nm") == 0) {
		zStepUnit = 0.001;
	}
	else if (unit.compare("um") == 0) {
		zStepUnit = 1;
	}
	else if (unit.compare("mm") == 0) {
		zStepUnit = 1000;
	}
	zStep = ui->zStepDoubleSpinBox->value() * zStepUnit;
}



void MainWindow::xStepUnitComboBox_changed()
{
	QString unit = ui->xStepUnitComboBox->currentText();
	if (unit.compare("nm") == 0) {
		xStepUnit = 0.001;
	}
	else if (unit.compare("um") == 0) {
		xStepUnit = 1;
	}
	else if (unit.compare("mm") == 0) {
		xStepUnit = 1000;
	}
	xStep = ui->xStepDoubleSpinBox->value() * xStepUnit;
}



void MainWindow::yStepUnitComboBox_changed()
{
	QString unit = ui->yStepUnitComboBox->currentText();
	if (unit.compare("nm") == 0) {
		yStepUnit = 0.001;
	}
	else if (unit.compare("um") == 0) {
		yStepUnit = 1;
	}
	else if (unit.compare("mm") == 0) {
		yStepUnit = 1000;
	}
	yStep = ui->yStepDoubleSpinBox->value() * yStepUnit;
}

void MainWindow::updatePrintProcessBar(int value, int max)
{
	ui->PrintProgressBar->setRange(0, max);
	ui->PrintProgressBar->setValue(value);
}


void MainWindow::savePatternConfiguration()
{
	QString fileName = QFileDialog::getSaveFileName(this, "Save file", QString(), "Text files (*.txt)");
	if (!fileName.isEmpty()) 
	{
		QFile file(fileName);
		if (file.open(QIODevice::WriteOnly | QIODevice::Text)) 
		{
			QTextStream stream(&file);
			for (int i = 0; i < m_elements.size(); ++i)
			{
				PatternElement pattern = m_elements.at(i);
				stream << pattern.name
					<< " " << pattern.exposure
					<< " " << pattern.darkPeriod
					<< " " << pattern.trigIn
					<< " " << pattern.trigOut2
					<< endl;
			}
		}
		file.close();
	}
}

void MainWindow::loadPatternConfiguration()
{
	m_elements.clear();
	QString fileName = QFileDialog::getOpenFileName(this, "Save file", QString(), "Text files (*.txt)");
	if (!fileName.isEmpty())
	{
		QFile file(fileName);
		if (file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			QTextStream in(&file);

			// 逐行读取文件内容
			while (!in.atEnd())
			{
				QString line = in.readLine();

				// 解析每行内容为一个Person对象
				QStringList fields = line.split(" ");
				if (fields.size() == 5)
				{
					PatternElement pattern;
					pattern.name = fields[0];
					pattern.exposure = fields[1].toInt();
					pattern.darkPeriod = fields[2].toInt();
					pattern.trigIn = (fields[3] == "1") ? true : false;
					pattern.trigOut2 = (fields[4] == "1") ? true : false;
					pattern.bits = 1;
					pattern.color = PatternElement::RED;
					pattern.splashImageBitPos = 0;
					pattern.splashImageIndex = 0;
					pattern.clear = true;
					pattern.selected = true;
					// 将Person对象添加到QList中
					m_elements.append(pattern);
				}
			}
			file.close();
		}
	}
	update_listWidget();
}

void MainWindow::update_listWidget()
{
	ui->listWidget->clear();
	layerNum = 0;
	for (int i = 0; i < m_elements.size(); i++)
	{
		QListWidgetItem *imageItem = new QListWidgetItem;
		imageItem->setSizeHint(QSize(275, 90));
		imageItem->setIcon(QIcon(m_elements[i].name));
		QFileInfo fileInfo(m_elements[i].name);
		imageItem->setText(fileInfo.fileName());
		ui->listWidget->setIconSize(QSize(160, 90));
		ui->listWidget->addItem(imageItem);
		QWidget *widget = new QWidget();
		QCheckBox *checkBox = new QCheckBox(widget);
		checkBox->setObjectName("checkBox");  // 设置 QCheckBox 对象的名称
		checkBox->setText("Trig");
		checkBox->setChecked(m_elements[i].trigIn);
		ui->listWidget->setItemWidget(imageItem, widget);
		connect(checkBox, &QCheckBox::clicked, this, [this]() {
			this->listWidgetCheckBox_changed();
		});
		if (m_elements[i].trigIn)
			layerNum += 1;
	}
	ui->LayerNumberSpinBox->setValue(layerNum);
}