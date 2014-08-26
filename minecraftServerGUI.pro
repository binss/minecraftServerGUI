#-------------------------------------------------
#
# Project created by QtCreator 2014-08-22T13:07:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = minecraftServerGUI
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    usermanager.cpp

HEADERS  += widget.h \
    usermanager.h

FORMS    += widget.ui
