# This file is part of Syntro
#
# Copyright (c) 2013 Pansenti, LLC. All rights reserved.
#

greaterThan(QT_MAJOR_VERSION, 4): cache()

TEMPLATE = app

TARGET = SyntroCVCam

DESTDIR = Output

QT += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += debug_and_release

unix {
	macx {
		LIBS += /usr/local/lib/libSyntroLib.dylib \
			/usr/local/lib/libSyntroGUI.dylib \

		INCLUDEPATH += /usr/local/include/syntro \
				/usr/local/include/syntro/SyntroAV

		target.path = /usr/local/bin
	}
	else {
		CONFIG += link_pkgconfig
		PKGCONFIG += syntro opencv
		target.path = /usr/bin
	}

	INSTALLS += target

	# No debug in release builds
	QMAKE_CXXFLAGS_RELEASE -= -g
}

DEFINES += QT_NETWORK_LIB

INCLUDEPATH += GeneratedFiles

MOC_DIR += GeneratedFiles/release

OBJECTS_DIR += release

UI_DIR += GeneratedFiles

RCC_DIR += GeneratedFiles

include(SyntroCVCam.pri)
