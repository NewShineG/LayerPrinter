#ifndef MAINWINDOW_H
#define MAINWINDOW_H



// 配置文件路径
#define SETTINGSPATH "./settings.ini"



#include <windows.h>
#include <QMainWindow>
#include <QCoreApplication>
#include <QMessageBox>
#include <QTime>
#include <QTimer>
#include <QFileDialog>
#include <QProgressDialog>
#include <QSettings>
#include <QDebug>
#include <QtAlgorithms>
#include <QTextStream>


#include "patternelement.h"
#include "PtnImage.h"
#include "DMDLib/API.h"
#include "DMDLib/common.h"
#include "DMDLib/usb.h"
#include "DMDLib/batchfile.h"
#include "PI_GCS2_DLL.h"




#if defined(_MSC_VER) && (_MSC_VER >= 1600)
# pragma execution_character_set("utf-8")
#endif

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class WorkThread;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
	~MainWindow();

	friend class WorkThread;
	WorkThread *printWork;

	//计时器
	QTimer *PosTimer;
	// 程序设置
	QSettings settings;
	//多图片路径
	QString m_ptnImagePath;
	//多图片信息容器
	QList<PatternElement> m_elements;
	//QListWidget listWidget;

	//曝光时间
	int expoTime;	//us
	//暗光时间
	int darkTime;	//us
	//步长
	double zStep;	//um
	double xStep;
	double yStep;
	//位置
	double zPos = 0;		//um
	double xPos = 0;
	double yPos = 0;
	//目标位置
	double zTarget = -1;		//um
	double xTarget = -1;
	double yTarget = -1;
	//速度
	double zVelocity = 1000000;		// um/s
	//单位
	int expoTimeUnit = 1;
	int darkTimeUnit = 1;
	double zStepUnit = 1;
	double xStepUnit = 1;
	double yStepUnit = 1;
	//DMD相关设置
	bool isRepeat = true;
	bool isTrigIn = false;
	bool isTrigOut = false;

	uint32 m_ptnWidth, m_ptnHeight;

	bool m_videoPatternMode = false;
	bool m_patternImageChange;
	bool m_dualAsic = false;

	//PI E709
	char stageController[200];
	int devicenumber = 0;
	char szIDN[200];
	int ID0 = -1;
	int ID1 = -1;
	int ID2 = -1;
	bool update_zTarget = false;
	BOOL trueFlag = true;
	BOOL falseFlag = false;

	//结构参数
	double structHeight = 0;
	double layerHeight = 0;
	//double startPos = 0;
	//double endPos = 250;
	int layerNum = 0;
	bool isStepMove = true;

	//手动连接DMD
	void DMDConnect();
	//获取DMD状态
	void getStatus();
	//添加图片
	void addPatterns();
	//清除图片
	void clearPatterns();
	//上传LUT
	void updateLUT();
	int updatePatternMemory(int totalSplashImages, BOOL firmware);
	int calculateSplashImageDetails(int * totalSplashImages, bool firmware);
	int uploadPatternToEVM(bool master, int splashImageCount, int splash_size, uint08 * splash_block);
	//DMD放映开始
	void startPatSequence();
	//DMD放映暂停
	void pausePatSequence();
	//DMD放映停止
	void stopPatSequence();
	//输出错误信息
	void showError(const char * str);

	//位移台连接
	void stageConnect();
	bool AutoZeroIfNeeded(int ID, char* axis);
	void zTargetMove();
	void zAdd();
	void zMinus();
	void zStop();
	void zVelocityMove();

	void xTargetMove();
	void xAdd();
	void xMinus();

	void yTargetMove();
	void yAdd();
	void yMinus();

	void updatePos();


	void startLayerPrint();
	void finishedLayerPrint();
	void stopLayerPrint();
	//Debug函数
	void on_DebugButton_clicked();

	void window_started();
	void expoTimeUnitComboBox_changed();
	void expoTimeSpinBox_changed();
	void darkTimeUnitComboBox_changed();
	void darkTimeSpinBox_changed();
	void DMDRepeatCheckBox_changed();
	void DMDPlayOnceCheckBox_changed();
	void DMDTrigInCheckBox_changed();
	void DMDTrigOutCheckBox_changed();
	void zStepUnitComboBox_changed();
	void xStepUnitComboBox_changed();	
	void yStepUnitComboBox_changed();
	void update_listWidget();
	void listWidgetCheckBox_changed();
	void savePatternConfiguration();
	void loadPatternConfiguration();

private:
    Ui::MainWindow *ui;

	private slots:
		void updatePrintProcessBar(int value, int max);

};



#endif // MAINWINDOW_H