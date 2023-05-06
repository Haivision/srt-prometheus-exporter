#
# SRT - Secure, Reliable, Transport
# Copyright (c) 2022 Sony Group Corporation.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

CC = gcc

INSTALL_DIR = /usr/local
LIB_INSTALL_DIR = $(INSTALL_DIR)/lib
LIB_CURL_DIR = /usr/lib/x86_64-linux-gnu/


CFLAGS = -I$(INSTALL_DIR)
LDFLAGS = -Wl,-rpath=$(LIB_INSTALL_DIR)
LDFLAGS += -L$(LIB_INSTALL_DIR)
LDFLAGS += -L$(LIB_CURL_DIR)
LIBS = -lsrtexp
LIBS += -lsrt
LIBS += -lyaml-cpp

SAMPLE_APP_SRT_SOURCE = sample_srtsource
SAMPLE_APP_SRT_DESTINATION = sample_srtdestination
SRC_SOURCE = sample_srtsource.c
SRC_DESTINATION = sample_srtdestination.c

all: srt

srt: $(SAMPLE_APP_SRT_SOURCE) $(SAMPLE_APP_SRT_DESTINATION)

$(SAMPLE_APP_SRT_SOURCE): $(SRC_SOURCE)
	$(CC) -o $@ $(CFLAGS) $(SRC_SOURCE) $(LDFLAGS) $(LIBS)

$(SAMPLE_APP_SRT_DESTINATION): $(SRC_DESTINATION)
	$(CC) -o $@ $(CFLAGS) $(SRC_DESTINATION) $(LDFLAGS) $(LIBS)

clean:
	-rm $(SAMPLE_APP_SRT_SOURCE)
	-rm $(SAMPLE_APP_SRT_DESTINATION)

.PHONY: all srt clean
