#ifndef WORKTHREAD_H
#define WORKTHREAD_H
#include <QThread>
#include "mainwindow.h"

#include "PI_GCS2_DLL.h"

#include<vector>
#include <fstream>

using namespace std;


#pragma execution_character_set("utf-8")
class WorkThread : public QThread
{
	Q_OBJECT
protected:
	void  run() Q_DECL_OVERRIDE;  //线程任务
public:
	WorkThread(MainWindow *p);

	double startPos = 0;
	double endPos = 250;
	bool isQuit = false;
	MainWindow *mw;
	void stop();

signals:
	void updatePrintProcess(int value,int max);
signals:
	void finished();
signals:
	void zTarget_add_layerHeight(); 


};

#endif // SAVETHREAD_H

