#-------------------------------------------------
#
# Project created by QtCreator 2012-12-16T22:12:34
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = objviewer
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11 -DDEBUG -fopenmp -Isrc/include -g
QMAKE_CXXFLAGS += `Magick++-config --cppflags` `pkg-config --cflags opencv`
LIBS += `Magick++-config --libs` `pkg-config --libs opencv` -fopenmp


SOURCES += Qt.cpp\
        mainwindow.cpp\
		src/color.cc\
		src/lib/*.cc src/renderable/*.cc src/space.cc\
		src/surface.cc src/texture.cc\
		src/view.cc src/mesh_simplifier.cc src/cvrender.cc

HEADERS  += mainwindow.h src/include/geometry/*.hh src/include/material/*.hh\
	src/include/renderable/*.hh src/include/*.hh

FORMS    += mainwindow.ui

DESTDIR = ./
OBJECTS_DIR = build/obj
MOC_DIR = build/moc
RCC_DIR = build/rcc
UI_DIR = build/ui
