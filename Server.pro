#-------------------------------------------------
#
# Project created by QtCreator 2016-08-26T00:17:16
#
#-------------------------------------------------

QT       += core gui\
            network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Server
TEMPLATE = app


SOURCES += main.cpp\
        server.cpp \
    view.cpp

HEADERS  += server.h \
    view.h

LIBS += `pkg-config opencv --libs`

FORMS    += \
    server.ui
