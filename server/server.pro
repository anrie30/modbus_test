QT += serialbus widgets

TEMPLATE = app
CONFIG += c++11

LIBS += -lmodbus

TARGET = ../bin/modbus_server
OBJECTS_DIR = ./obj
MOC_DIR = ./obj

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    main.cpp \
    modbusserver.cpp

HEADERS += \
    modbusserver.h
