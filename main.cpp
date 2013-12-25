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

#include "SyntroLib.h"
#include "SyntroCVCam.h"

#include <QApplication>
#include <QtDebug>
#include <QSettings>

#ifdef Q_OS_LINUX
#include "SyntroCVCamConsole.h"
int runConsoleApp(int argc, char *argv[]);
#endif

int runGuiApp(int argc, char *argv[]);


int main(int argc, char *argv[])
{
	bool wantConsole = SyntroUtils::checkConsoleModeFlag(argc, argv);

#ifdef Q_OS_LINUX
	if (wantConsole)
		return runConsoleApp(argc, argv);
	else
		return runGuiApp(argc, argv);
#else
	if (wantConsole) {
		printf("Console mode for this app not supported on this platform\n");
		exit(1);
	}
		
	return runGuiApp(argc, argv);
#endif
}

int runGuiApp(int argc, char *argv[])
{
	QApplication a(argc, argv);

	SyntroUtils::loadStandardSettings(PRODUCT_TYPE, a.arguments());

	SyntroCVCam *w = new SyntroCVCam();

	w->show();

	return a.exec();
}

#ifdef Q_OS_LINUX
int runConsoleApp(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	bool daemonMode = SyntroUtils::checkDaemonModeFlag(argc, argv);

	SyntroUtils::loadStandardSettings(PRODUCT_TYPE, a.arguments());

	SyntroCVCamConsole wc(daemonMode, &a);

	return a.exec();
}
#endif
