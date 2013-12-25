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

#include <qglobal.h>
#include <qsettings.h>

#include "LogWrapper.h"

#include "OpenCVCamera.h"
#include "SyntroCVCam.h"


OpenCVCamera::OpenCVCamera()
{
	m_logTag = PRODUCT_TYPE;
}

OpenCVCamera::~OpenCVCamera()
{
	closeDevice();
}

bool OpenCVCamera::getNextFrame(Mat& frame)
{
	if (!m_vidcap.isOpened()) {
		m_vidcap.release();
		openDevice();
		return false;
	}

	m_vidcap >> frame;

	return !frame.empty();
}

bool OpenCVCamera::openDevice()
{
	QSettings *settings = SyntroUtils::getSettings();

	// set some defaults if they are not already present

	if (!settings->contains(SYNTRO_CAMERA_CAMERA))
		settings->setValue(SYNTRO_CAMERA_CAMERA, 0);

	// only apply to Mac
	if (!settings->contains(SYNTRO_CAMERA_WIDTH))
		settings->setValue(SYNTRO_CAMERA_WIDTH, 640);

	if (!settings->contains(SYNTRO_CAMERA_HEIGHT))
		settings->setValue(SYNTRO_CAMERA_HEIGHT, 480);

	m_device = settings->value(SYNTRO_CAMERA_CAMERA, 0).toInt();
	m_width = settings->value(SYNTRO_CAMERA_WIDTH, 640).toInt();
	m_height = settings->value(SYNTRO_CAMERA_HEIGHT, 480).toInt();

	delete settings;

	if (m_device < 0) {
		logError(QString("Invalid device %1").arg(m_device));
		return false;
	}
		
	if (!m_vidcap.open(m_device)) {
		logError(QString("Failed to open camera %1").arg(m_device));
		return false;
	}

#ifdef Q_OS_MAC
	if (!m_vidcap.set(CV_CAP_PROP_FRAME_WIDTH, m_width)) {
		logWarn(QString("Failed to set frame width %1").arg(m_width));
	}

	if (!m_vidcap.set(CV_CAP_PROP_FRAME_HEIGHT, m_height)) {
		logWarn(QString("Failed to set frame height %1").arg(m_height));
	}
#endif
	return true;
}

void OpenCVCamera::closeDevice()
{
	if (m_vidcap.isOpened())
		m_vidcap.release();
}
