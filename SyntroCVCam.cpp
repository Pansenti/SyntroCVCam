//
//  Copyright (c) 2012, 2013 Pansenti, LLC.
//	
//  This file is part of Syntro
//
//  Syntro is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  Syntro is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with Syntro.  If not, see <http://www.gnu.org/licenses/>.
//

#include "SyntroCVCam.h"
#include "SyntroAboutDlg.h"
#include "BasicSetupDlg.h"
#include "CameraClient.h"
#include "OpenCVCamera.h"

SyntroCVCam::SyntroCVCam()
	: QMainWindow()
{
	m_logTag = PRODUCT_TYPE;

	ui.setupUi(this);

	layoutStatusBar();

	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(ui.actionAbout, SIGNAL(triggered()), this, SLOT(onAbout()));
	connect(ui.actionBasicSetup, SIGNAL(triggered()), this, SLOT(onBasicSetup()));

	restoreWindowState();

	m_frameCount = 0;
	m_frameRateTimer = 0;
	m_frameRefreshTimer = 0;

	SyntroUtils::syntroAppInit();

	m_client = new CameraClient(this);
	m_client->resumeThread();

	// Use a direct connection since we can't simply transfer an OpenCV Mat object
	// as signal/slot argument without some extra work. This is good enough for a
	// simple Syntro example.
	connect(this, SIGNAL(newFrame(Mat)), m_client, SLOT(newFrame(Mat)), Qt::DirectConnection);

	m_frameRateTimer = startTimer(STAT_REFRESH_SECS * SYNTRO_CLOCKS_PER_SEC);
	m_frameRefreshTimer = startTimer(SYNTRO_CLOCKS_PER_SEC / FRAME_RATE_DIVISOR);

	m_camera = new OpenCVCamera();

	setWindowTitle(QString("%1 - %2")
		.arg(SyntroUtils::getAppType())
		.arg(SyntroUtils::getAppName()));
}

void SyntroCVCam::closeEvent(QCloseEvent *)
{
	if (m_camera) {
		delete m_camera;
		m_camera = NULL;
	}

	if (m_frameRateTimer) {
		killTimer(m_frameRateTimer);
		m_frameRateTimer = 0;
	}

	if (m_frameRefreshTimer) {
		killTimer(m_frameRefreshTimer);
		m_frameRefreshTimer = 0;
	}

	m_client->exitThread();

	SyntroUtils::syntroAppExit();

	saveWindowState();
}

void SyntroCVCam::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == m_frameRateTimer)
		doStatusUpdate();
	else
		doImageUpdate();
}

void SyntroCVCam::doStatusUpdate()
{
	m_controlStatus->setText(m_client->getLinkState());

	m_frameRateStatus->setText(QString().sprintf("%0.1lf fps",
		(double)m_frameCount / STAT_REFRESH_SECS));

	m_frameCount = 0;
}

void SyntroCVCam::doImageUpdate()
{
	Mat frame;

	if (!m_camera->getNextFrame(frame))
		return;

	m_frameCount++;

	if (!isMinimized())
		showImage(&frame);

	emit newFrame(frame);
}

void SyntroCVCam::showImage(Mat *frame)
{
	QImage img((const uchar*) frame->data, frame->cols, frame->rows, (int)frame->step, QImage::Format_RGB888);
	QImage swappedImg = img.rgbSwapped();					// sadly BGR has to be swapped to RGB
	ui.cameraView->setPixmap(QPixmap::fromImage(swappedImg.scaled(ui.cameraView->size(), Qt::KeepAspectRatio)));
}

void SyntroCVCam::layoutStatusBar()
{
	m_controlStatus = new QLabel(this);
	m_controlStatus->setAlignment(Qt::AlignLeft);
	ui.statusBar->addWidget(m_controlStatus, 1);

	m_frameRateStatus = new QLabel(this);
	m_frameRateStatus->setAlignment(Qt::AlignCenter | Qt::AlignLeft);
	ui.statusBar->addPermanentWidget(m_frameRateStatus);
}

void SyntroCVCam::onAbout()
{
	SyntroAbout dlg(this);
	dlg.exec();
}

void SyntroCVCam::onBasicSetup()
{
	BasicSetupDlg dlg(this);
	dlg.exec();
}

void SyntroCVCam::saveWindowState()
{
	QSettings *settings = SyntroUtils::getSettings();

	settings->beginGroup("Window");
	settings->setValue("Geometry", saveGeometry());
	settings->setValue("State", saveState());
	settings->endGroup();
	
	delete settings;
}

void SyntroCVCam::restoreWindowState()
{
	QSettings *settings = SyntroUtils::getSettings();

	settings->beginGroup("Window");
	restoreGeometry(settings->value("Geometry").toByteArray());
	restoreState(settings->value("State").toByteArray());
	settings->endGroup();
	
	delete settings;
}
