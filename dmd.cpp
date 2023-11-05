#include "mainwindow.h"
#include "ui_mainwindow.h"

//手动DMD连接
void MainWindow::DMDConnect()
{
	// 检测DMD连接状态并尝试进行连接
	if (USB_IsConnected() == false)
	{
		USB_Open();
	}
	if (USB_IsConnected())
	{
		getStatus();
		ui->DMDConnectCheckBox->setChecked(1);
		ui->DMDConnectCheckBox->setText("已连接");
	}
	else
	{
		ui->DMDConnectCheckBox->setChecked(0);
		ui->DMDConnectCheckBox->setText("未连接");
	}
}



/**
 * @brief MainWindow::on_addPatternsButton_clicked
 */
void MainWindow::addPatterns()
{
	int i;
	int numPatAdded = 0;


	QStringList fileNames;

	fileNames = QFileDialog::getOpenFileNames(this,
			QString("Select Image for Pattern"),
			m_ptnImagePath,
			"*.bmp *.png *.tiff *.tif");
	if (fileNames.isEmpty())
			return;

	fileNames.sort();

	QDir dir = QFileInfo(QFile(fileNames.at(0))).absoluteDir();
	m_ptnImagePath = dir.absolutePath();
	settings.setValue("m_tnImagePath", m_ptnImagePath);

	for (i = 0; i < m_elements.size(); i++)
		m_elements[i].selected = false;

	for (i = 0; i < fileNames.size(); i++)
		{
#if 0
			QByteArray ba = fileNames.at(i).toLocal8Bit();
			int width, height;

			BMP_GetImageSize(ba.data(), &width, &height);
			if ((width != m_ptnWidth) || (height != m_ptnHeight))
			{

				if (m_dualAsic)
					sprintf(errMsg, "Error: Cannot add image %s. Image resolution permitted is 2560 * 1600", ba.data());
				else
					sprintf(errMsg, "Error: Cannot add image %s. Image resolution permitted is 1920 * 1080", ba.data());

				showCriticalError(errMsg);
				continue;
			}
#endif
		PatternElement pattern;

		if (m_elements.size() == 0)
		{
			pattern.bits = 1;
			pattern.color = PatternElement::RED;
			pattern.exposure = (expoTime > 105) ? expoTime : 105;
			pattern.darkPeriod = darkTime;
			pattern.trigIn = isTrigIn;
			pattern.trigOut2 = isTrigOut;
			pattern.splashImageBitPos = 0;
			pattern.splashImageIndex = 0;
			pattern.clear = true;
		}
		else
		{
			pattern.bits = m_elements[m_elements.size() - 1].bits;
			pattern.color = m_elements[m_elements.size() - 1].color;
			pattern.exposure = m_elements[m_elements.size() - 1].exposure;
			pattern.darkPeriod = m_elements[m_elements.size() - 1].darkPeriod;
			pattern.trigIn = m_elements[m_elements.size() - 1].trigIn;
			pattern.trigOut2 = m_elements[m_elements.size() - 1].trigOut2;
			pattern.clear = m_elements[m_elements.size() - 1].clear;
			//if (!ui->autoupdate_checkBox->isChecked())
			{
				pattern.splashImageIndex = m_elements[m_elements.size() - 1].splashImageIndex;
				pattern.splashImageBitPos = m_elements[m_elements.size() - 1].splashImageBitPos;
			}
		}


		pattern.name = fileNames.at(i);
		pattern.selected = true;

		m_elements.append(pattern);
		numPatAdded++;
		m_patternImageChange = true;
	}

	if (m_elements.size() > 0)
		update_listWidget();
	return;
}

void MainWindow::listWidgetCheckBox_changed()
{
	layerNum = 0;
	QStringList itemList;
	//遍历当前的listwidget
	for (int i = 0; i < ui->listWidget->count(); i++)
	{
		QListWidgetItem *item = ui->listWidget->item(i);
		//将QWidget 转化为QCheckBox  获取第i个item 的控件
		QWidget *widget = static_cast<QWidget *>(ui->listWidget->itemWidget(item));
		QCheckBox *foundCheckBox = widget->findChild<QCheckBox*>("checkBox");
		if (foundCheckBox) {
			// 找到了 QCheckBox 对象
			// 获取 QCheckBox 对象的 checked 属性
			m_elements[i].trigIn = foundCheckBox->isChecked();
		}
		if (m_elements[i].trigIn)
		{
			layerNum += 1;
			ui->LayerNumberSpinBox->setValue(layerNum);
		}
	}
}


void MainWindow::clearPatterns()
{
	m_elements.clear();
	ui->listWidget->clear();
}

/**
 * @brief MainWindow::on_updateLUT_Button_clicked
 */
void MainWindow::updateLUT()
{
	int totalSplashImages = 0;
	int ret;
	QTime waitEndTime;
	char errStr[255];

	if (LCR_SetMode(PTN_MODE_OTF) < 0)
	{
		showError("DMD: Unable to switch to pattern mode");
		return;
	}

	if (m_elements.size() <= 0)
	{
		showError("Error:No pattern sequence to send");
		return;
	}

	LCR_ClearPatLut();

	if (!m_videoPatternMode)
	{
		//if (ui->autoupdate_checkBox->isChecked())
		//{
		if (calculateSplashImageDetails(&totalSplashImages, FALSE))
			return;
		//}
	}

	for (int i = 0; i < m_elements.size(); i++)
	{
		if (LCR_AddToPatLut(i, m_elements[i].exposure, m_elements[i].clear, m_elements[i].bits, m_elements[i].color, m_elements[i].trigIn, m_elements[i].darkPeriod, m_elements[i].trigOut2, m_elements[i].splashImageIndex, m_elements[i].splashImageBitPos) < 0)
		{
			sprintf(errStr, "Unable to add pattern number %d to the LUT", i);
			showError(errStr);
			break;
		}
	}

	if (LCR_SendPatLut() < 0)
	{
		showError("Sending pattern LUT failed!");
		return;
	}

	if (isRepeat)
		ret = LCR_SetPatternConfig(m_elements.size(), 0);
	else
		ret = LCR_SetPatternConfig(m_elements.size(), m_elements.size());

	if (ret < 0)
	{
		showError("Sending pattern LUT size failed!");
		return;
	}

	//if (ui->patternMemory_radioButton->isChecked() && m_patternImageChange)
	{
		if (updatePatternMemory(totalSplashImages, false) == 0)
		{
			m_patternImageChange = false;
		}
	}
}

/**
 * @brief MainWindow::UpdatePatternMemory
 * Creates Splash images from all the Pattern elements
 * If it is Firmware update, adds the splash images to the firmware
 * If on the fly, converts the splash images to splash blocks and updates on teh fly
 * @param totalSplashImages - I - total number of Splash images to be updated in Firmware
 * @param firmware - I - boolean to determine if it is to update firmware or On the Fly mode
 * @return
 */
int MainWindow::updatePatternMemory(int totalSplashImages, BOOL firmware)
{
	uint32 m_ptnWidth = 1920;
	uint32 m_ptnHeight = 1080;
#if IMG_DEBUG
	QImageWriter writer("D:/imagepostmerge.png", "PNG");
	QImageWriter writer2("D:/imageMASTER.png", "PNG");
	QImageWriter writer3("D:/imageSLAVE.png", "PNG");
#endif
	//Stop timer
	PosTimer->stop();
	for (int image = 0; image < totalSplashImages; image++)
	{

		int splashImageCount;

		if (firmware)
		{
			splashImageCount = image;
		}
		else
		{
			splashImageCount = totalSplashImages - 1 - image;
		}

		PtnImage merge_image(m_ptnWidth, m_ptnHeight, 24, PTN_RGB24);

		merge_image.fill(0);

		int i;
		int es = m_elements.size();
		for (i = 0; i < es; i++)
		{
			int ei = m_elements[i].splashImageIndex;
			if (ei != splashImageCount)
				continue;
			int bitpos = m_elements[i].splashImageBitPos;
			int bitdepth = m_elements[i].bits;
			PtnImage image(m_elements[i].name);
			merge_image.merge(image, bitpos, bitdepth);
		}
#if IMG_DEBUG
		QImage output = merge_image.qimage();
		writer.write(output);
#endif
		merge_image.swapColors(PTN_COLOR_RED, PTN_COLOR_BLUE, PTN_COLOR_GREEN);
		uint08* splash_block = NULL;

		PtnImage merge_image_master(m_ptnWidth, m_ptnHeight, 24, PTN_RGB24);
		PtnImage merge_image_slave(m_ptnWidth, m_ptnHeight, 24, PTN_RGB24);
		merge_image_master = merge_image;
		merge_image_slave = merge_image;
		if (m_dualAsic)
		{
			//
			//			merge_image_master.crop(0, 0, m_ptnWidth / 2, m_ptnHeight);
			//			merge_image_slave.crop(m_ptnWidth / 2, 0, m_ptnWidth / 2, m_ptnHeight);
			//#if IMG_DEBUG
			//			output = merge_image_master.qimage();
			//			writer2.write(output);
			//			output = merge_image_slave.qimage();
			//			writer3.write(output);
			//#endif
			//			if (firmware == true)
			//			{
			//				//if (m_firmware->addSplash(&merge_image_master, (SPL_Compression_t)ui->comboBox_imageCompression->currentIndex()) < 0)
			//				//{
			//				//	showError(GET_ERR_STR());
			//				//	return -1;
			//				//}
			//				//if (m_firmwareSlave != NULL)
			//				//{
			//				//	if (m_firmwareSlave->addSplash(&merge_image_slave, (SPL_Compression_t)ui->comboBox_imageCompression->currentIndex()) < 0)
			//				//	{
			//				//		showError(GET_ERR_STR());
			//				//		return -1;
			//				//	}
			//				//}
			//			}
			//
			//			else
			//			{
			//				uint08* splash_block_master = NULL;
			//				uint08* splash_block_slave = NULL;
			//
			//				int splashSizeMaster = merge_image_master.toSplash(&splash_block_master, (SPL_Compression_t)ui->comboBox_imageCompression->currentIndex());
			//				int splashSizeSlave = merge_image_slave.toSplash(&splash_block_slave, (SPL_Compression_t)ui->comboBox_imageCompression->currentIndex());
			//
			//				if (splashSizeMaster <= 0 || splashSizeSlave <= 0)
			//					return -1;
			//
			//				if (uploadPatternToEVM(true, splashImageCount, splashSizeMaster, splash_block_master) == -1)
			//					return -1;
			//
			//				if (uploadPatternToEVM(false, splashImageCount, splashSizeSlave, splash_block_slave) == -1)
			//					return -1;
			//			}
		}
		else
		{
			if (firmware == true)
			{
				//if (m_firmware->addSplash(&merge_image, (SPL_Compression_t)ui->comboBox_imageCompression->currentIndex()) < 0)
				//{
				//	showError(GET_ERR_STR());
				//	return -1;
				//}
			}
			else

			{
				int splashSize = merge_image.toSplash(&splash_block, (SPL_Compression_t)3);
				if (splashSize <= 0)
					return -1;
				if (uploadPatternToEVM(true, splashImageCount, splashSize, splash_block) < 0)
					return -1;
			}
		}
	}

	PosTimer->start();

	return 0;
}

/**
 * @brief MainWindow::UpdateLUTOnTheFly
 * Updates the Pattern images into the Splash block on the Firmware image in the EVM on the fly
 * @param master - I - boolean to indicate if it is madetr or slave
 * @param splashImageCount - I - the Index of the Splash Image to be updated
 * @param splash_size - I - size of the splash image that is being updated
 * @param splash_block - I - the updated splash block
 * @return
 */
int MainWindow::uploadPatternToEVM(bool master, int splashImageCount, int splash_size, uint08* splash_block)
{
	int origSize = splash_size;

	LCR_InitPatternMemLoad(master, splashImageCount, splash_size);

	QProgressDialog imgDataDownload("Image data download", "Abort", 0, splash_size, this);
	imgDataDownload.setWindowTitle(QString("Pattern Data Download.."));
	imgDataDownload.setWindowModality(Qt::WindowModal);
	imgDataDownload.setLabelText(QString("Uploading to EVM"));
	imgDataDownload.setValue(0);
	int imgDataDwld = 0;
	imgDataDownload.setMaximum(origSize);
	imgDataDownload.show();
	QApplication::processEvents();
	while (splash_size > 0)
	{
		int dnldSize = LCR_pattenMemLoad(master, splash_block + (origSize - splash_size), splash_size);
		if (dnldSize < 0)
		{
			// free(imageBuffer);
			showError("Downloading failed");
			PosTimer->start();
			imgDataDownload.close();
			return -1;
		}

		splash_size -= dnldSize;

		if (splash_size < 0)
			splash_size = 0;

		imgDataDwld += dnldSize;
		imgDataDownload.setValue(imgDataDwld);
		QApplication::processEvents();
		if (imgDataDownload.wasCanceled())
		{
			imgDataDownload.setValue(splash_size);
			imgDataDownload.close();
			return -1;
		}
	}

	QApplication::processEvents();
	imgDataDownload.close();

	return 0;

}

/**
 * @brief MainWindow::calculateSplashImageDetails
 * for each of the pattern image on the pattern settings page, calculates the
 * total number of splash images of bit depth 24 based on the bit depth of each image
 * Also calculates the bitposition of each pattern element in the splash Image
 * and the index of the Splash image for each Pattern element
 * @param totalSplashImages - O - Total number of splash images to be created from
 *                                the available Pattern images
 * @return - 0 - success
 *          -1 - failure
 */
int MainWindow::calculateSplashImageDetails(int *totalSplashImages, bool firmware)
{
	int maxbits = 400;
	int imgCount = 0;
	int bits = 0;
	int totalBits = 0;
	for (int elemCount = 0; elemCount < m_elements.size(); elemCount++)
	{
		if (m_elements[elemCount].bits > 16)
		{
			char dispStr[255];
			sprintf(dispStr, "Error:Bit depth not selected for pattern=%d\n", elemCount);
			showError(dispStr);
			return -1;
		}

		totalBits = totalBits + m_elements[elemCount].bits;

		if (firmware == TRUE)
			maxbits = 3984;
		if (totalBits > maxbits)
		{
			char dispStr[255];
			if (firmware == FALSE)
				sprintf(dispStr, "Error:Total Bit Depth cannot exceed 400");
			else
				sprintf(dispStr, "Error:Total Bit Depth cannot exceed 3984");
			showError(dispStr);
			return -1;
		}

		/* Check if the same pattern is used already */
		int i;
		for (i = 0; i < elemCount; i++)
		{
			/* Only if file name and bit depth matches */
			if (m_elements[i].bits == m_elements[elemCount].bits &&
				m_elements[i].name == m_elements[elemCount].name)
			{
				break;
			}
		}

		/* Match found. use the same splash image */
		if (i < elemCount)
		{
			m_elements[elemCount].splashImageIndex = m_elements[i].splashImageIndex;
			m_elements[elemCount].splashImageBitPos = m_elements[i].splashImageBitPos;
			continue;
		}

		/* If it is the last image or cant fit in the current image */
		if (elemCount == m_elements.size() ||
			(bits + m_elements[elemCount].bits) > 24)
		{
			/* Goto next image */
			imgCount++;
			bits = 0;
		}

		m_elements[elemCount].splashImageIndex = imgCount;
		m_elements[elemCount].splashImageBitPos = bits;
		bits += m_elements[elemCount].bits;
	}

	*totalSplashImages = imgCount + 1;

	return 0;
}

/**
 * @brief MainWindow::on_startPatSequence_Button_clicked
 */
void MainWindow::startPatSequence()
{
	/*
		unsigned int repeat;

		if(ui->repeat_radioButton->isChecked())
			repeat = 0;
		else
			repeat = m_elements.size();

		if(LCR_SetPatternConfig(m_elements.size(), repeat)<0)
		{
			showError("Error in setting LUT Configuration!");
			return;
		}
	*/
	if (LCR_PatternDisplay(0x2) < 0)
		showError("Unable to stat pattern display");
}
/**
 * @brief MainWindow::on_pausePatSequence_Button_clicked
 */
void MainWindow::pausePatSequence()
{
	if (LCR_PatternDisplay(0x1) < 0)
		showError("Unable to pause pattern display");
}
/**
 * @brief MainWindow::on_stopPatSequence_Button_clicked
 */
void MainWindow::stopPatSequence()
{
	if (LCR_PatternDisplay(0x0) < 0)
		showError("Unable to stop pattern display");
}
