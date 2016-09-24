#-------------------------------------------------
#
# Project created by QtCreator 2016-09-11T17:55:40
#
#-------------------------------------------------

QT       += core gui network

DEFINES += __print="\"qDebug()<< __FILE__ << __LINE__ <<  Q_FUNC_INFO\""

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UDPChat
TEMPLATE = app


SOURCES += main.cpp\
    user.cpp \
    server.cpp

HEADERS  += \
    user.h \
    server.h

FORMS    += dialog.ui
