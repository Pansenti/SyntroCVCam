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

#include <termios.h>
#include <unistd.h>
#include <signal.h>

#include "SyntroCVCamConsole.h"
#include "SyntroCVCam.h"
#include "CameraClient.h"

volatile bool SyntroCVCamConsole::sigIntReceived = false;

SyntroCVCamConsole::SyntroCVCamConsole(bool daemonMode, QObject *parent)
    : QThread(parent)
{
	m_daemonMode = daemonMode;

    m_logTag = PRODUCT_TYPE;
	m_fps = 0;
	m_frameCount = 0;

	m_camera = NULL;

	if (m_daemonMode) {
		registerSigHandler();

		if (daemon(1, 1)) {
			perror("daemon");
			return;
		}
	}

    SyntroUtils::syntroAppInit();

    m_client = new CameraClient(this);
	m_client->resumeThread();

	if (!m_daemonMode)
		m_frameRateTimer = 0;
	else
		m_frameRateTimer = startTimer(STAT_REFRESH_SECS * SYNTRO_CLOCKS_PER_SEC);

    m_frameRefreshTimer = startTimer(SYNTRO_CLOCKS_PER_SEC / FRAME_RATE_DIVISOR);

    start();
}

void SyntroCVCamConsole::aboutToQuit()
{
	if (m_frameRateTimer) {
		killTimer(m_frameRateTimer);
		m_frameRateTimer = 0;
	}

	if (m_frameRefreshTimer) {
		killTimer(m_frameRefreshTimer);
		m_frameRefreshTimer = 0;
	}

	for (int i = 0; i < 5; i++) {
		if (wait(1000))
			break;

		if (!m_daemonMode)
			printf("Waiting for console thread to finish...\n");
	}

	if (m_camera) {
		delete m_camera;
		m_camera = NULL;
	}
}

void SyntroCVCamConsole::timerEvent(QTimerEvent *event)
{
    Mat frame;

    if (!m_running)
        return;

    if (event->timerId() == m_frameRateTimer) {
        m_fps = (double)m_frameCount / STAT_REFRESH_SECS;
        m_frameCount = 0;
    } 
    else {
        if (!m_camera->getNextFrame(frame))
            return;

        m_frameCount++;

        emit newFrame(frame);
    }
}

void SyntroCVCamConsole::showHelp()
{
	printf("\nOptions are:\n\n");
	printf("  H - Show help\n");
	printf("  S - Show status\n");
	printf("  X - Exit\n");
}

void SyntroCVCamConsole::showStatus()
{
	printf("\nSyntroControl status : %s\n", qPrintable(m_client->getLinkState()));
	printf("Frame rate : %0.2lf fps\n", m_fps);
}

void SyntroCVCamConsole::run()
{
    m_camera = new OpenCVCamera();

	connect(this, SIGNAL(newFrame(Mat)), m_client, SLOT(newFrame(Mat)), Qt::DirectConnection);

	m_running = true;

	if (m_daemonMode)
		runDaemon();
	else
		runConsole();

	m_running = false;

	m_client->exitThread();

	SyntroUtils::syntroAppExit();

	QCoreApplication::exit();
}

void SyntroCVCamConsole::runConsole()
{
    struct termios ctty;

    tcgetattr(fileno(stdout), &ctty);
    ctty.c_lflag &= ~(ICANON);
    tcsetattr(fileno(stdout), TCSANOW, &ctty);

    while (m_running) {
		printf("\nEnter option: ");

        switch (toupper(getchar())) {
			case 'H':
				showHelp();
				break;

			case 'S':
				showStatus();
				break;

			case 'X':
				printf("\nExiting\n");
                m_running = false;
				break;

			case '\n':
				continue;
		}
	}
}

void SyntroCVCamConsole::runDaemon()
{
	while (!SyntroCVCamConsole::sigIntReceived)
		msleep(100);
}

void SyntroCVCamConsole::registerSigHandler()
{
	struct sigaction sia;

	bzero(&sia, sizeof sia);
	sia.sa_handler = SyntroCVCamConsole::sigHandler;

	if (sigaction(SIGINT, &sia, NULL) < 0)
		perror("sigaction(SIGINT)");
}

void SyntroCVCamConsole::sigHandler(int)
{
	SyntroCVCamConsole::sigIntReceived = true;
}
