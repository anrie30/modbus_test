QT += widgets network

CONFIG += c++11

TARGET = ../bin/collector
OBJECTS_DIR = ./obj
MOC_DIR = ./obj

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    tcpclient.cpp

HEADERS += \
    mainwindow.h \
    tcpclient.h

FORMS += \
    mainwindow.ui
