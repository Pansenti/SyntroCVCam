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

#ifndef SYNTRO_OPENCVCAMERA_H
#define SYNTRO_OPENCVCAMERA_H

#include <qstring.h>

#include <opencv2/highgui/highgui.hpp>
using namespace cv;


class OpenCVCamera
{
public:
	OpenCVCamera();
	virtual ~OpenCVCamera();

	bool getNextFrame(Mat &frame);
	bool openDevice();
	void closeDevice();

private:
	VideoCapture m_vidcap;
	int m_device;
	int m_width;
	int m_height;

	QString m_logTag;
};

#endif // SYNTRO_OPENCVCAMERA_H