QT += core gui widgets

CONFIG += c++11

TARGET = ../bin/collector
OBJECTS_DIR = ./obj
MOC_DIR = ./obj

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui
