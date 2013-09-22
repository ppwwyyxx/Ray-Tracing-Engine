# $File: Makefile
# $Date: Sun Sep 22 18:43:24 2013 +0800

OBJ_DIR = build
TARGET = ray-tracing

INCLUDE_DIR = -Iinclude
DEFINES = -DDEBUG

OPTFLAGS = -Wall -Wextra -O3

LIBS = opencv
INCLUDE_DIR += $(shell pkg-config --cflags $(LIBS))

CXXFLAGS = $(INCLUDE_DIR)
CXXFLAGS += $(shell Magick++-config --cxxflags)
CXXFLAGS += $(DEFINES) -std=c++11 $(OPTFLAGS) -fopenmp -pthread
LDFLAGS = $(shell pkg-config $(LIBS) --libs)
LDFLAGS += $(OPTFLAGS)
LDFLAGS += $(shell Magick++-config --ldflags --libs) -fopenmp -pthread

CC = g++
SHELL = bash
ccSOURCES = $(shell find -name "*.cc" | sed 's/^\.\///g')
OBJS = $(addprefix $(OBJ_DIR)/,$(ccSOURCES:.cc=.o))
DEPFILES = $(OBJS:.o=.dep)

.PHONY: all clean run gui

all: $(TARGET)

sinclude $(DEPFILES)

$(OBJ_DIR)/%.o: %.cc
	@echo "[cc] $< ..."
	@$(CC) -c $< -o $@ $(CXXFLAGS)

$(OBJ_DIR)/%.dep: %.cc  Makefile
	@mkdir -p $(dir $@)
	@echo "[dep] $< ..."
	@$(CC) $(CXXFLAGS) $(INCLUDE_DIR) $(DEFINES) -MM -MT "$(OBJ_DIR)/$(<:.cc=.o) $(OBJ_DIR)/$(<:.cc=.dep)" "$<"  > "$@"


$(TARGET): $(OBJS)
	@echo "Linking ..."
	@$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo "done."

gui:
	cd gui && qmake
	make -C gui

clean:
	@rm -rf $(OBJ_DIR) gui/Makefile

run: $(TARGET)
	./$(TARGET)

