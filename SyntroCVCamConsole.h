//
//  Copyright (c) 2012 Pansenti, LLC.
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

#ifndef SYNTROCVCAMCONSOLE_H
#define SYNTROCVCAMCONSOLE_H

#include "QThread"
#include "CameraClient.h"
#include "OpenCVCamera.h"

class SyntroCVCamConsole : public QThread
{
	Q_OBJECT

public:
    SyntroCVCamConsole(bool daemonMode, QObject *parent);

signals:
	void newFrame(Mat frame);

public slots:
	void aboutToQuit();

protected:
	void timerEvent(QTimerEvent *event);
	void run();

private:
	void showHelp();
	void showStatus();
	void runConsole();
	void runDaemon();
	void registerSigHandler();
	static void sigHandler(int sig);

	static volatile bool sigIntReceived;
	bool m_daemonMode;

	CameraClient *m_client;
	OpenCVCamera *m_camera;

	int m_frameRefreshTimer;
	int m_frameRateTimer;
	int m_frameCount;
	double m_fps;

	QString m_logTag;

	bool m_running;
};

#endif // SYNTROCVCAMCONSOLE_H

