#-------------------------------------------------
#
# Project created by QtCreator 2012-12-16T22:12:34
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Q3DViewer
TEMPLATE = app

QMAKE_CXXFLAGS += -O3 -std=c++11 -Isrc/include -DDEBUG -fopenmp `Magick++-config --cppflags`
LIBS += `Magick++-config --libs` -fopenmp


SOURCES += Qt.cpp\
        mainwindow.cpp\
		src/color.cc\
		src/lib/*.cc src/renderable/*.cc src/space.cc\
		src/surface.cc src/texture.cc\
		src/view.cc src/mesh_simplifier.cc

HEADERS  += mainwindow.h src/include/geometry/*.hh src/include/material/*.hh\
	src/include/renderable/*.hh src/include/*.hh

FORMS    += mainwindow.ui

DESTDIR = build
OBJECTS_DIR = build/obj
MOC_DIR = build/moc
RCC_DIR = build/rcc
UI_DIR = build/ui
