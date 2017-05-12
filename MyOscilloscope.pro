#-------------------------------------------------
#
# Project created by QtCreator 2016-07-18T10:08:11
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = myOscilloscope
TEMPLATE = app


SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/plot.cpp \
    src/wheelbox.cpp \
    src/curvedata.cpp \
    src/samplingthread.cpp \
    src/PlotCurve.cpp

HEADERS  += \
    include/mainwindow.h \
    include/plot.h \
    include/wheelbox.h \
    include/curvedata.h \
    include/samplingthread.h \
    include/PlotCurve.h

FORMS    +=

#QWT_LIB_DIR = "C:\Qt\Qt5.7.0\5.7\mingw53_32\lib"
#QWT_INCLUDE_DIR = "C:\Qt\Qt5.7.0\5.7\mingw53_32\include\Qwt"

QWT_LIB_DIR = "C:\Qt\Qt4.8.7\mingw482\lib"
QWT_INCLUDE_DIR = "C:\Qt\Qt4.8.7\mingw482\include\Qwt"

LIBS += -L$${QWT_LIB_DIR} \
        -lqwt \
        -lqwtd
INCLUDEPATH += $${QWT_INCLUDE_DIR} \
               ./include \
               ./src

OBJECTIVE_SOURCES +=
