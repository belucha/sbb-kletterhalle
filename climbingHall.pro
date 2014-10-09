#-------------------------------------------------
#
# Project created by QtCreator 2014-10-05T20:01:45
#
#-------------------------------------------------

QT       += core gui network

QMAKE_CXXFLAGS+= -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = climbingHall
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    json11.cpp

HEADERS  += mainwindow.h \
    json11.hpp

FORMS    += mainwindow.ui

OTHER_FILES += \
    README.md

RESOURCES += \
    resources.qrc
