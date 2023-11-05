#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"workThread.h"
#include <string>
#include <exception>
#include <iostream>


char zAxes[] = "Z";

void MainWindow::stageConnect()
{
	PI_EnumerateUSB(stageController, 199, "E-709");
	if (!PI_IsConnected(ID0))
		ID0 = PI_ConnectUSB(stageController);
	if (!PI_IsConnected(ID0)) {
		ui->stageConnectCheckBox->setChecked(0);
		ui->stageConnectCheckBox->setText("未连接");
		return;
	}
	if (ID0 < 0)
	{
		showError("StageConnect: Failed!");
		ui->stageConnectCheckBox->setChecked(0);
		ui->stageConnectCheckBox->setText("未连接");
		return;
	}
	if (!PI_qSAI_ALL(ID0, zAxes, 10))
		showError("StageConnect:Get zAxes Failed!");
	BOOL SVOFlag;
	if(!PI_qSVO(ID0, zAxes, &SVOFlag))
		showError("StageConnect: Get PI_qSVO Failed!");
	if (!SVOFlag) 
	{
		if (!PI_SVO(ID0, zAxes, &trueFlag))
		{
			PI_CloseConnection(ID0);
			showError("StageConnect: Set Servo Failed!");
			ui->stageConnectCheckBox->setChecked(0);
			ui->stageConnectCheckBox->setText("未连接");
			return;
		}	
	}
	if (!AutoZeroIfNeeded(ID0, zAxes))
	{
		PI_CloseConnection(ID0);
		showError("StageConnect: Not referenced, Referencing failed.");
		return;
	}
	ui->stageConnectCheckBox->setChecked(1);
	ui->stageConnectCheckBox->setText("已连接");
	
}

bool MainWindow::AutoZeroIfNeeded(int ID, char* axis)
{
	BOOL bAutoZeroed;

	if (!PI_qATZ(ID, axis, &bAutoZeroed))
	{
		return false;
	}

	if (!bAutoZeroed)
	{
		// if needed, autozero the axis
		std::cout << "AutoZero axis " << axis << "..." << std::endl;

		BOOL bUseDefaultVoltageArray[1];
		bUseDefaultVoltageArray[0] = TRUE;

		if (!PI_ATZ(ID, axis, NULL, bUseDefaultVoltageArray))
		{
			return false;
		}

		// Wait until the autozero move is done.
		BOOL bFlag = FALSE;

		while (bFlag != TRUE)
		{
			if (!PI_IsControllerReady(ID, &bFlag))
			{
				return false;
			}
		}
	}

	std::cout << "AutoZero successfully" << std::endl;

	return true;
}

//void MainWindow::zAdd()
//{
//	if (zPos + zStep > 250)
//	{
//		showError("zAdd:Beyond the limit!");
//		return;
//	}
//	zTarget = zPos + zStep;
//	BOOL pbValueArray = FALSE; 
//	int i = 1;
//	if (!PI_MOV(ID0, zAxes, &zTarget))//z方向正向调整一个步长
//	{
//		showError("zAdd:Fail to Move!");
//		return;
//	}
//	while (!pbValueArray) 
//	{
//		PI_qONT(ID0, zAxes, &pbValueArray);
//		Sleep(1);
//		if (i > 5000)
//			break;
//		i += 1;
//	}
//}

//void MainWindow::zMinus()
//{
//	if (zPos - zStep < 0)
//	{
//		showError("zMinus:Beyond the limit!");
//		return;
//	}
//	zTarget = zPos - zStep;
//	BOOL pbValueArray = FALSE;
//	int i = 1;
//	if (!PI_MOV(ID0, zAxes, &zTarget))//z方向负向调整一个步长
//	{
//		showError("zMinus:Fail to Move!");
//		return;
//	}
//	while (!pbValueArray)
//	{
//		PI_qONT(ID0, zAxes, &pbValueArray);
//		Sleep(1);
//		if (i > 5000)
//			break;
//		i += 1;
//	}
//}

void MainWindow::zAdd()
{
	if ((zTarget + zStep) <= 250)
		zTarget = zTarget + zStep;
	ui->zTargetDoubleSpinBox->setValue(zTarget);
	zTargetMove();
}

void MainWindow::zMinus()
{
	if((zTarget-zStep) >= 0)
		zTarget = zTarget - zStep;
	ui->zTargetDoubleSpinBox->setValue(zTarget);
	zTargetMove();
}

void MainWindow::zTargetMove()
{
	
	if (zTarget > 250)
	{
		showError("zAdd:Beyond the limit!");
		return;
	}
	else if (zTarget < 0)
	{
		showError("zMinus:Beyond the limit!");
		return;
	}
	BOOL isReachedArray = FALSE;
	int i = 1;
	if (!PI_MOV(ID0, zAxes, &zTarget))				//z轴移动到目标位置
	{
		showError("zTargetMove:Fail to Move!");
		return;
	}
	while (!isReachedArray)
	{
		PI_qONT(ID0, zAxes, &isReachedArray);
		Sleep(1);
		if (i > 5000)
			break;
		i += 1;
	}
}

void MainWindow::zStop()
{
	if (!PI_STP(ID0))
		showError("PI_STP: Stop zAxes Failed!");
}

void MainWindow::zVelocityMove()
{
	if (!PI_VEL(ID0, zAxes, &zVelocity)) 
	{
		showError("PI_VEL: Set zVelocity Failed!");
		return;
	}
	if (!PI_MOV(ID0, zAxes, &zTarget))				//z轴移动到目标位置
	{
		showError("zTargetMove:Fail to Move!");
		return;
	}
}

void MainWindow::xTargetMove()
{
}

void MainWindow::xAdd()
{
}

void MainWindow::xMinus()
{
}

void MainWindow::yTargetMove()
{
}

void MainWindow::yAdd()
{
}

void MainWindow::yMinus()
{
}



void MainWindow::updatePos()
{
	if (ID0 > -1)PI_qPOS(ID0, zAxes, &zPos);
	if (ID1 > -1)PI_qPOS(ID1, zAxes, &xPos); 
	if (ID2 > -1)PI_qPOS(ID2, zAxes, &yPos);
	ui->zPosLabel->setText(QString::number(zPos, 10, 3));
	ui->xPosLabel->setText(QString::number(xPos, 10, 3));
	ui->yPosLabel->setText(QString::number(yPos, 10, 3));
	if (zTarget < 0)
	{
		zTarget = round(zPos);
		ui->zTargetDoubleSpinBox->setValue(zTarget);
	}
	if (update_zTarget) {
		ui->zTargetDoubleSpinBox->setValue(zTarget);
	}
}

void MainWindow::startLayerPrint()
{

	ui->startLayerPrintButton->setEnabled(0);
	ui->stopLayerPrintButton->setEnabled(1);
	printWork = new WorkThread(this);
	connect(printWork, SIGNAL(updatePrintProcess(int, int)), this, SLOT(updatePrintProcessBar(int, int)));
	connect(printWork, &WorkThread::finished, this, [this] {
		finishedLayerPrint();
	});
	ui->PrintProgressBar->setRange(0, layerNum);
	ui->PrintProgressBar->setValue(0);
	printWork->start();
}

void MainWindow::stopLayerPrint()
{
	//zStop();
	//stopPatSequence();
	//ui->startLayerPrintButton->setEnabled(1);
	//ui->stopLayerPrintButton->setEnabled(0);
	printWork->stop();
}

void MainWindow::finishedLayerPrint()
{
	//打印结束，上移安全距离
	//zTarget = zTarget + 5;
	//zTargetMove();
	zStop();
	stopPatSequence();
	ui->startLayerPrintButton->setEnabled(1);
	ui->stopLayerPrintButton->setEnabled(0);
	ui->zTargetDoubleSpinBox->setValue(zTarget);
	printWork->deleteLater();
}


void MainWindow::on_DebugButton_clicked()
{
	if (!PI_qVEL(ID0, zAxes, &zVelocity))
	{
		showError("PI_qVEL: Get zVelocity Failed!");
		return;
	}
	ui->zVelocityDoubleSpinBox->setValue(zVelocity);

}