#-------------------------------------------------
#
# Project created by QtCreator 2012-12-16T22:12:34
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Q3DViewer
TEMPLATE = app

QMAKE_CXXFLAGS += -O3 -std=c++11 -Isrc/include -DDEBUG


SOURCES += Qt.cpp\
        mainwindow.cpp\
		src/color.cc\
		src/debugutils.cc src/light.cc src/plane.cc src/space.cc\
		src/sphere.cc src/surface.cc src/texture.cc src/utils.cc\
		src/view.cc

HEADERS  += mainwindow.h src/include/geometry/*.hh src/include/material/*.hh\
	src/include/renderable/*.hh src/include/*.hh

FORMS    += mainwindow.ui

DESTDIR = build
OBJECTS_DIR = build/obj
MOC_DIR = build/moc
RCC_DIR = build/rcc
UI_DIR = build/ui
