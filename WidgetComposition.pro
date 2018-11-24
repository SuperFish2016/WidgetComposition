#-------------------------------------------------
#
# Project created by QtCreator 2018-08-20T16:44:15
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MFTool
TEMPLATE = app
CONFIG+=sdk_no_version_check
# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

MOC_DIR = haha
SOURCES += \
        main.cpp \
        mainwindow.cpp \
    graphics/graphicsscenebase.cpp \
    graphics/graphicsviewscalable.cpp \
    graphics/graphicswidgetcomposition.cpp \
    graphics/graphicswidgetsegment.cpp \
    graphics/graphicswidgetsequence.cpp \
    graphics/graphicswidgettimeline.cpp \
    graphics/graphicswidgetresources.cpp \
    graphics/graphicscommon.cpp \
    imf/imfcommon.cpp \
    graphics/graphicsscene.cpp \
    widget/widgetcomposition.cpp \
    widget/widgettrackdetails.cpp \
    utils/commands.cpp \
    imf/imfpackage.cpp

HEADERS += \
        mainwindow.h \
    graphics/graphicsviewscalable.h \
    graphics/graphicswidgetcomposition.h \
    graphics/graphicswidgetsegment.h \
    graphics/graphicswidgetsequence.h \
    graphics/graphicswidgettimeline.h \
    widget/widgetcomposition.h \
    graphics/graphicswidgetresources.h \
    graphics/graphicscommon.h \
    graphics/graphicsscene.h \
    imf/imfcommon.h \
    widget/widgettrackdetails.h \
    utils/commands.h \
    imf/imfpackage.h

FORMS += \
        mainwindow.ui

RESOURCES += \
    resources/resources.qrc

DISTFILES += \
    graphics/ReadMe.txt
