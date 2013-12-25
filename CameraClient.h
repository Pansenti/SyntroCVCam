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

#ifndef CAMERACLIENT_H
#define CAMERACLIENT_H

#include "SyntroLib.h"
#include "SyntroCVCam.h"

#include <opencv2/highgui/highgui.hpp>
using namespace cv;

#define	CAMERA_IMAGE_INTERVAL	((qint64)SYNTRO_CLOCKS_PER_SEC / 40)

class CameraClient : public Endpoint
{
	Q_OBJECT

public:
	CameraClient(QObject *parent);

public slots:
	void newFrame(Mat frame);

protected:
	void appClientInit();
	void appClientBackground();

private:
	void sendFrame();

	int m_cameraPort;

	QMutex m_frameMutex;
	Mat m_frame;
	bool m_haveNewFrame;

	SYNTRO_AVPARAMS m_avParams;
	int m_recordIndex;

	vector<int>	m_jpgEncoderParams;
};

#endif // CAMERACLIENT_H
