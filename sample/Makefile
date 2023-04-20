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

SAMPLE_APP_SRT_SOURCE_C = sample_app_srt_source_c
SAMPLE_APP_SRT_DESTINATION_C = sample_app_srt_destination_c
SRCSOURCE = sample_srtsource.c
SRCDESTINATION = sample_srtdestination.c

all: srt

srt: $(SAMPLE_APP_SRT_SOURCE_C) $(SAMPLE_APP_SRT_DESTINATION_C)

$(SAMPLE_APP_SRT_SOURCE_C): $(SRCSOURCE)
	$(CC) -o $@ $(CFLAGS) $(SRCSOURCE) $(LDFLAGS) $(LIBS)

$(SAMPLE_APP_SRT_DESTINATION_C): $(SRCDESTINATION)
	$(CC) -o $@ $(CFLAGS) $(SRCDESTINATION) $(LDFLAGS) $(LIBS)

clean:
	-rm $(SAMPLE_APP_SRT_SOURCE_C)
	-rm $(SAMPLE_APP_SRT_DESTINATION_C)

.PHONY: all srt clean
