#
# SRT - Secure, Reliable, Transport
# Copyright (c) 2022 Sony Group Corporation.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

CXX = g++

CFLAGS = -I./include
CFLAGS += -I./export
LDFLAGS = -shared
LDFLAGS += -fPIC
LDFLAGS += -Wl,-rpath-link
LIBS = -lsrt
LIBS += -lyaml-cpp
LIBS += -lprometheus-cpp-core
LIBS += -lprometheus-cpp-pull

SRT_EXPORTER_LIB = libsrtexp.so
INSTALL_DIR = /usr/local
LIB_INSTALL_DIR = $(INSTALL_DIR)/lib
INC_INSTALL_DIR = $(INSTALL_DIR)/include
CFG_INSTALL_DIR = /etc

INC_DIR = include
EXP_DIR = export
SRC_DIR = source
OBJ_DIR = source
SRT_EXPORTER_HEAD = $(wildcard $(INC_DIR)/*.hpp) $(wildcard $(EXP_DIR)/*.hpp) $(wildcard $(EXP_DIR)/*.h)
SRT_EXPORTER_SRC = $(wildcard $(SRC_DIR)/*.cpp)
SRT_EXPORTER_OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRT_EXPORTER_SRC))

all: target install

target: $(SRT_EXPORTER_LIB)

$(SRT_EXPORTER_LIB): $(SRT_EXPORTER_OBJ) $(SRT_EXPORTER_HEAD)
	$(CXX) -o $@ $(SRT_EXPORTER_OBJ) $(LDFLAGS) $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(SRT_EXPORTER_HEAD)
	$(CXX) $(CFLAGS) $(LDFLAGS) -c -o $@ $<

install: $(SRT_EXPORTER_LIB)
	mkdir -p $(INC_INSTALL_DIR)/srtexp
	mkdir -p $(CFG_INSTALL_DIR)/srtexp
	cp $(SRT_EXPORTER_LIB) $(LIB_INSTALL_DIR)
	cp ./export/*.* $(INC_INSTALL_DIR)/srtexp
	cp ./config/*.* $(CFG_INSTALL_DIR)/srtexp

clean:
	-rm $(SRC_DIR)/*.o
	-rm libsrtexp.so
	-rm $(LIB_INSTALL_DIR)/$(SRT_EXPORTER_LIB)
	-rm -rf $(INC_INSTALL_DIR)/srtexp
	-rm -rf $(CFG_INSTALL_DIR)/srtexp

.PHONY: all target install clean
