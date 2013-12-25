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

#include "CameraClient.h"


CameraClient::CameraClient(QObject *)
		: Endpoint(CAMERA_IMAGE_INTERVAL, COMPTYPE_CAMERA)
{
	m_jpgEncoderParams.push_back(CV_IMWRITE_JPEG_QUALITY);
	m_jpgEncoderParams.push_back(75);

	// setup for mjpeg, no audio
	memset(&m_avParams, 0, sizeof(m_avParams));

	m_recordIndex = 0;

	m_avParams.avmuxSubtype = SYNTRO_RECORD_TYPE_AVMUX_MJPPCM;
	m_avParams.videoSubtype = SYNTRO_RECORD_TYPE_VIDEO_MJPEG;
	m_avParams.audioSubtype = SYNTRO_RECORD_TYPE_AUDIO_UNKNOWN;

	// just a fudge, we aren't controlling framerate with OpenCV
	m_avParams.videoFramerate = 20;

	// fill in video width and height when we receive frames
}

void CameraClient::appClientInit()
{
	m_cameraPort = clientAddService(SYNTRO_STREAMNAME_AVMUX, SERVICETYPE_MULTICAST, true);
	m_haveNewFrame = false;
}

void CameraClient::newFrame(Mat frame)
{
	if (m_frameMutex.tryLock()) {
		m_frame = frame;
		m_haveNewFrame = !m_frame.empty();
		m_frameMutex.unlock();
	}
}

void CameraClient::appClientBackground()
{
	if (!clientIsServiceActive(m_cameraPort))
		return;

	if (!clientClearToSend(m_cameraPort))
		return;

	if (m_haveNewFrame)
		sendFrame();
}

void CameraClient::sendFrame()
{
	vector<uchar> jpeg;

	// we are only here if m_newFrame is true
	// get what we need from m_frame and release the lock
	m_frameMutex.lock();

	// use an OpenCV function to convert the image to jpeg
	imencode(".jpg", m_frame, jpeg, m_jpgEncoderParams);

	// This doesn't change, but we don't know for sure until we process the
	// first frame. No big deal, just update every time.
	m_avParams.videoWidth = m_frame.cols;
	m_avParams.videoHeight = m_frame.rows;

	m_haveNewFrame = false;

	m_frameMutex.unlock();

	// total packet length
	int length = sizeof(SYNTRO_RECORD_AVMUX) + (int)jpeg.size();

	// Request a Syntro a mulicast packet with extra room for our data
	// Our data consists of a video header and the actual image
	// Syntro will fill in the SYNTRO_EHEAD multicast header
	SYNTRO_EHEAD *multiCast = clientBuildMessage(m_cameraPort, length);

	// the avmux data starts after the multicast header
	SYNTRO_RECORD_AVMUX *avmuxHead = (SYNTRO_RECORD_AVMUX *)(multiCast + 1);
	
	// A utility function fills in the avmux header for us
	SyntroUtils::avmuxHeaderInit(avmuxHead, &m_avParams, SYNTRO_RECORDHEADER_PARAM_NORMAL,
		m_recordIndex++, 0, (int)jpeg.size(), 0);

	// Since we only have image data, append it right after the avmux header
	memcpy((unsigned char *)(avmuxHead + 1), &jpeg[0], jpeg.size());

	// send it
	clientSendMessage(m_cameraPort, multiCast, length, SYNTROLINK_MEDPRI);
}
