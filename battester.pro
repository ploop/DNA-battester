#-------------------------------------------------
#
# Project created by QtCreator 2017-07-22T15:00:10
#
#-------------------------------------------------

QT       += core \
            gui \
            serialport \
            printsupport \

unix:LIBS += -ludev
win32:LIBS += -lhid -lsetupapi


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = battester
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp \
    custommod.cpp \
    mods/dnamod.cpp \
    mods/nuvotonmod.cpp

unix:SOURCES += lib/hid_linux.c
win32:SOURCES += lib/hid_windows.c

HEADERS  += mainwindow.h \
    qcustomplot.h \
    custommod.h \
    mods/dnamod.h \
    mods/nuvotonmod.h \
    lib/hidapi.h

FORMS    += mainwindow.ui

RESOURCES = battester.qrc

TRANSLATIONS += \
    translations/battester_ru.ts


tr.commands = lupdate $$_PRO_FILE_ && lrelease $$_PRO_FILE_
    PRE_TARGETDEPS += tr
    QMAKE_EXTRA_TARGETS += tr
