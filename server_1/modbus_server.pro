QT -= gui
QT += core network

CONFIG += c++11
TEMPLATE = app

TARGET = ../bin/modbus_server
OBJECTS_DIR = ./obj
MOC_DIR = ./obj

LIBS += -lmodbus

SOURCES += main.cpp \
    modbusreceiver.cpp \
    modbustcpserver.cpp

HEADERS += \
    modbuscontext.h \
    modbusreceiver.h \
    modbustcpserver.h
