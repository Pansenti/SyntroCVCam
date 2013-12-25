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

#ifndef SyntroCVCam_H
#define SyntroCVCam_H

#include "SyntroLib.h"

#include <qmainwindow.h>
#include "ui_syntrocvcam.h"
#include <opencv2/core/core.hpp>
using namespace cv;

#define STAT_REFRESH_SECS 2
#define FRAME_RATE_DIVISOR 50

#define	PRODUCT_TYPE "SyntroCVCam"

// Settings keys
#define	SYNTRO_CAMERA_CAMERA		"Camera"
#define	SYNTRO_CAMERA_WIDTH		"Width"
#define	SYNTRO_CAMERA_HEIGHT		"Height"

// forward declarations
class CameraClient;
class OpenCVCamera;

class SyntroCVCam : public QMainWindow
{
	Q_OBJECT

public:
	SyntroCVCam();

public slots:
	void onAbout();
	void onBasicSetup();

signals:
	void newFrame(Mat frame);

protected:
	void timerEvent(QTimerEvent *event);
	void closeEvent(QCloseEvent *event);

private:
	void doStatusUpdate();
	void doImageUpdate();
	void showImage(Mat *frame);
	void layoutStatusBar();
	void saveWindowState();
	void restoreWindowState();

	CameraClient *m_client;

	Ui::SyntroCVCamClass ui;

	QLabel *m_frameRateStatus;
	QLabel *m_controlStatus;

	int m_frameRateTimer;
	int m_frameRefreshTimer;
	int m_frameCount;

	OpenCVCamera *m_camera;

	QString m_logTag;
};

#endif // SyntroCVCam_H
