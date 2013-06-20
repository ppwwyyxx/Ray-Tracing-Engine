# File: objviewer.pro
# Date: Thu Jun 20 16:43:09 2013 +0800
# Author: Yuxin Wu <ppwwyyxxc@gmail.com>

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = objviewer
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11 -DDEBUG -fopenmp -I../include -O3
QMAKE_CXXFLAGS += `Magick++-config --cppflags` `pkg-config --cflags opencv`
LIBS += `Magick++-config --libs` `pkg-config --libs opencv` -fopenmp


SOURCES += main.cxx window.cxx\
		../color.cc\
		../lib/*.cc ../renderable/*.cc ../space.cc\
		../surface.cc ../texture.cc\
		../view.cc ../mesh_simplifier.cc ../cvrender.cc

HEADERS  += window.hh ../include/geometry/*.hh ../include/material/*.hh\
	../include/renderable/*.hh ../include/*.hh

FORMS    += window.ui

DESTDIR = ../
OBJECTS_DIR = ../build/obj
MOC_DIR = ../build/moc
RCC_DIR = ../build/rcc
UI_DIR = ../build/ui
