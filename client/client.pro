QT += serialbus widgets network

TEMPLATE = app
CONFIG += c++11

TARGET = ../bin/modbus_client
OBJECTS_DIR = ./obj
MOC_DIR = ./obj

SOURCES += \
    main.cpp\
    mainwindow.cpp \
    modbusclient.cpp \
    tcpserver.cpp

HEADERS += \
    mainwindow.h \
    modbusclient.h \
    tcpserver.h

FORMS += \
    mainwindow.ui
