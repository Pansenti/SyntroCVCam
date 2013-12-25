# This file is part of Syntro
#
# Copyright (c) 2013 Pansenti, LLC. All rights reserved.
#

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += CameraClient.h \
    OpenCVCamera.h \
    SyntroCVCam.h

unix:HEADERS += SyntroCVCamConsole.h

SOURCES += CameraClient.cpp \
    main.cpp \
    OpenCVCamera.cpp \
    SyntroCVCam.cpp

unix:SOURCES += SyntroCVCamConsole.cpp

FORMS += syntrocvcam.ui
