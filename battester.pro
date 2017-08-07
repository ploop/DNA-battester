#-------------------------------------------------
#
# Project created by QtCreator 2017-07-22T15:00:10
#
#-------------------------------------------------

QT       += core \
            gui \
            serialport \
            printsupport


unix:LIBS += -lhidapi-hidraw


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = battester
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp \
    custommod.cpp \
    mods/dnamod.cpp \
    mods/nuvotonmod.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h \
    custommod.h \
    mods/dnamod.h \
    mods/nuvotonmod.h

FORMS    += mainwindow.ui

