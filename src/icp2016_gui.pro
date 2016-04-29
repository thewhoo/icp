#-------------------------------------------------
#
# Project created by QtCreator 2016-04-26T08:46:08
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = hra2016-gui
TEMPLATE = app

LIBS += -L boost/lib -lboost_serialization
QMAKE_CXXFLAGS += -std=c++11 -isystem boost

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc