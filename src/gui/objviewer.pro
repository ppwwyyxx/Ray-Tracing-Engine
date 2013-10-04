# File: objviewer.pro
# Author: Yuxin Wu <ppwwyyxxc@gmail.com>

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = objviewer
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11 -DDEBUG -fopenmp -I../include -O3
QMAKE_CXXFLAGS += `Magick++-config --cppflags` `pkg-config --cflags opencv`
LIBS += `Magick++-config --libs` `pkg-config --libs opencv` -fopenmp


SOURCES += main.cxx window.cxx\
		../static_const.cc\
		../lib/*.cc ../renderable/*.cc\
		../render/*.cc ../view.cc ../mesh_simplifier.cc ../myrender.cc ../kdtree.cc

HEADERS += window.hh ../include/geometry/*.hh ../include/material/*.hh\
	../include/renderable/*.hh ../include/*.hh ../include/render/*.hh ../include/librender/*.hh

FORMS   += window.ui

DESTDIR = ../
OBJECTS_DIR = ../build/obj
MOC_DIR = ../build/moc
RCC_DIR = ../build/rcc
UI_DIR = ../build/ui
