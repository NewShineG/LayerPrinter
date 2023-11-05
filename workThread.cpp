#include "workThread.h"
#include <QDebug>
#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>

extern char zAxes[];

void WorkThread::run()
{
	long t = (mw->expoTime + mw->darkTime)*1.1;
	vector<long> waitTime;
	int j = -1;
	for (int i = 0; i < mw->m_elements.size(); i++) {
		if (mw->m_elements[i].trigIn) {
			if(i != 0)
				waitTime.push_back((i - j)*t);
			j = i;
		}
	}
	if(j >= 0)
		waitTime.push_back((mw->m_elements.size() - j)*t);
	int piTriggerOutputIds = 2;										//I/O:		Triger Out 2
	int piTriggerParameterArray[5];
	double piTriggerValueArray[5];
	startPos = mw->zPos;
	endPos = mw->zPos + mw->structHeight;
	if (mw->isStepMove)
	{
		/*
		CTO 1 2 1
		CTO 1 3 2
		*/
		piTriggerParameterArray[0] = 2;										//Axis:
		piTriggerValueArray[0] = 2;												//Axis1
		piTriggerParameterArray[1] = 3;										//Triggermode:
		piTriggerValueArray[1] = 2;												//Target Arrived
		const int piTriggerParameter0 = 3;
		const double piTriggerValue0 = 2;
		char errBuf[100];
		if (!PI_CTO(mw->ID0, &piTriggerOutputIds, &piTriggerParameter0, &piTriggerValue0, 1)) {
			mw->showError("PI_CTO:Set Trigger_Out Failed");
			int errNr = PI_GetError(mw->ID0);
			PI_TranslateError(errNr,errBuf,100);
		}			
		mw->startPatSequence();
		mw->update_zTarget = true;
		for (int i = 0; i < mw->layerNum; i++)
		{
			mw->zTarget = mw->zTarget + mw->layerHeight;
			mw->zTargetMove();
			usleep(waitTime[i]);
			emit updatePrintProcess(i+1, mw->layerNum);
			if (isQuit)
				break;
		}
		mw->update_zTarget = false;
	}
	else
	{
		/*
		CTO < TrigOutID> 2 Axis
		CTO < TrigOutID> 3 Triggermode
		CTO < TrigOutID> 1 Stepsize
		CTO < TrigOutID> 8 StartValue
		CTO < TrigOutID> 9 StopValue
		*/
		mw->zVelocity = mw->structHeight / ((mw->expoTime + mw->darkTime) * mw->m_elements.size());
		if (!PI_VEL(mw->ID0, zAxes, &(mw->zVelocity)))
		{
			mw->showError("PI_VEL: Set zVelocity Failed!");
			return;
		}
		piTriggerParameterArray[0] = 2;										//Axis:
		piTriggerValueArray[0] = 1;												//Axis1
		piTriggerParameterArray[1] = 3;										//Triggermode:
		piTriggerValueArray[1] = 0;												//Position Distance
		piTriggerParameterArray[2] = 1;										//Stepsize:
		piTriggerValueArray[2] = mw->layerHeight;						//layerHeight
		piTriggerParameterArray[3] = 8;										//StartValue:
		piTriggerValueArray[3] = 0;												//0
		piTriggerParameterArray[4] = 9;										//StopValue:
		piTriggerValueArray[4] = 250;											//250
		
		const int piTriggerParameter0 = 3;
		const double piTriggerValue0 = 0;
		const int piTriggerParameter1 = 1;
		const double piTriggerValue1 = mw->layerHeight;
		const int piTriggerParameter2 = 8;
		const double piTriggerValue2 = 0;
		const int piTriggerParameter3 = 9;
		const double piTriggerValue3 = 250;

		char errBuf[100];
		//if (!PI_CTO(mw->ID0, &piTriggerOutputIds, piTriggerParameterArray, piTriggerValueArray, 5)) {
		//	mw->showError("PI_CTO:Set Trigger_Out Failed");
		//	int errNr = PI_GetError(mw->ID0);
		//	PI_TranslateError(errNr, errBuf, 100);
		//}
		if (!PI_CTO(mw->ID0, &piTriggerOutputIds, &piTriggerParameter0, &piTriggerValue0, 1)) {
			mw->showError("PI_CTO:Set Trigger_Out Failed");
			int errNr = PI_GetError(mw->ID0);
			PI_TranslateError(errNr, errBuf, 100);
		}
		if (!PI_CTO(mw->ID0, &piTriggerOutputIds, &piTriggerParameter1, &piTriggerValue1, 1)) {
			mw->showError("PI_CTO:Set Trigger_Out Failed");
			int errNr = PI_GetError(mw->ID0);
			PI_TranslateError(errNr, errBuf, 100);
		}
		if (!PI_CTO(mw->ID0, &piTriggerOutputIds, &piTriggerParameter2, &piTriggerValue2, 1)) {
			mw->showError("PI_CTO:Set Trigger_Out Failed");
			int errNr = PI_GetError(mw->ID0);
			PI_TranslateError(errNr, errBuf, 100);
		}
		if (!PI_CTO(mw->ID0, &piTriggerOutputIds, &piTriggerParameter3, &piTriggerValue3, 1)) {
			mw->showError("PI_CTO:Set Trigger_Out Failed");
			int errNr = PI_GetError(mw->ID0);
			PI_TranslateError(errNr, errBuf, 100);
		}

		mw->startPatSequence();
		BOOL isReachedArray = FALSE;
		int i = 0;
		double zStart = mw->zTarget;
		mw->zTarget = mw->zTarget + mw->structHeight + mw->layerHeight;
		if (!PI_MOV(mw->ID0, zAxes, &(mw->zTarget)))				//z轴移动到目标位置
		{
			mw->showError("zTargetMove:Fail to Move!");
			//emit finished();
			//return;
		}
		while (!isReachedArray)
		{
			PI_qONT(mw->ID0, zAxes, &isReachedArray);
			usleep(10000);
			if (i > 3000)
				break;
			if (mw->ID0 > -1)PI_qPOS(mw->ID0, zAxes, &mw->zPos);
			emit updatePrintProcess((int)(mw->zPos - zStart), (int)(mw->zTarget - zStart));
			if (isQuit)
				break;
			i += 1;
		}
		
		mw->zVelocity = 1000000;
		if (!PI_VEL(mw->ID0, zAxes, &(mw->zVelocity)))
		{
			mw->showError("PI_VEL: Set zVelocity Failed!");
			return;
		}
		emit updatePrintProcess((int)(mw->zTarget - zStart), (int)(mw->zTarget - zStart));
	}

	const int piTriggerParameter = 1;										//Stepsize:
	const double piTriggerValue = 250;										//layerHeight
	char errBuf[100];
	if(!PI_CTO(mw->ID0, &piTriggerOutputIds, &piTriggerParameter, &piTriggerValue, 1)){
		mw->showError("PI_CTO:Set Trigger_Out Failed");
		int errNr = PI_GetError(mw->ID0);
		PI_TranslateError(errNr, errBuf, 100);
	}
	waitTime.clear();
	emit finished();
}

WorkThread::WorkThread(MainWindow *p)
{
	mw = p;//将主线程Widegt对象的地址赋值给子线程中的Widget对象，这样就可以访问主线程中的所有数据方法而不受线程的限制
}

void WorkThread::stop()
{
	if (isRunning()) 
	{
		isQuit = true;
		wait();
	}
}
