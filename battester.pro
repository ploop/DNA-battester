#-------------------------------------------------
#
# Project created by QtCreator 2017-07-22T15:00:10
#
#-------------------------------------------------

QT       += core \
            gui \
            serialport \
            printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = battester
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h

FORMS    += mainwindow.ui
