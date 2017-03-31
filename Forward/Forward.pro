#-------------------------------------------------
#
# Project created by QtCreator 2017-03-18T18:56:51
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Forward
TEMPLATE = app

LIBS += -I/usr/include/openssl -L/usr/local/lib -lssl -lcrypto -lm

SOURCES += main.cpp\
        mainwindow.cpp \
    middle.cpp \
    network.cpp \
    server.cpp \
    client.cpp \
    header.cpp \
    eventhandler.cpp \
    cache.cpp \
    globals.cpp \
    hateclock.cpp

HEADERS  += mainwindow.h \
    middle.h \
    network.h \
    server.h \
    client.h \
    header.h \
    eventhandler.h \
    cache.h \
    globals.h \
    hateclock.h

FORMS    += mainwindow.ui
