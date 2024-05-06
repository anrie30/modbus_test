QT += serialbus widgets

TEMPLATE = app
CONFIG += c++11

TARGET = ../bin/modbus_client
OBJECTS_DIR = ./obj
MOC_DIR = ./obj

SOURCES += \
    main.cpp\
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui
