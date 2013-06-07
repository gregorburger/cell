#-------------------------------------------------
#
# Project created by QtCreator 2013-06-04T13:37:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cell
TEMPLATE = app

DEFINES += THRUST_DEVICE_SYSTEM=THRUST_DEVICE_SYSTEM_OMP

QMAKE_CXXFLAGS += -fopenmp -std=c++11
QMAKE_LFLAGS += -fopenmp

INCLUDEPATH += thrust

SOURCES += main.cpp\
    graphicsview.cpp \
    cell.cpp

HEADERS  += \
    graphicsview.h \
    cell.h

FORMS    += mainwindow.ui
