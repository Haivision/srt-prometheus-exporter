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

SAMPLE_APP_SRT_CALLER = sample_srtcaller
SAMPLE_APP_SRT_LISTENER = sample_srtlistener
SRC_CALLER = sample_srtcaller.c
SRC_LISTENER = sample_srtlistener.c

all: srt

srt: $(SAMPLE_APP_SRT_CALLER) $(SAMPLE_APP_SRT_LISTENER)

$(SAMPLE_APP_SRT_CALLER): $(SRC_CALLER)
	$(CC) -o $@ $(CFLAGS) $(SRC_CALLER) $(LDFLAGS) $(LIBS)

$(SAMPLE_APP_SRT_LISTENER): $(SRC_LISTENER)
	$(CC) -o $@ $(CFLAGS) $(SRC_LISTENER) $(LDFLAGS) $(LIBS)

clean:
	-rm $(SAMPLE_APP_SRT_CALLER)
	-rm $(SAMPLE_APP_SRT_LISTENER)

.PHONY: all srt clean
