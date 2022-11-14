/*
 * SRT - Secure, Reliable, Transport
 * Copyright (c) 2022 Sony Group Corporation.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <srt/srt.h>


typedef enum {
    SRT_EXP_C_STDERR = 0,
    SRT_EXP_C_SYSLOG = 1,
} SrtExpCLogDestination;

typedef enum {
    SRT_EXP_C_SUCCESS = 0,
    SRT_EXP_C_FAILURE = -1,
    SRT_EXP_C_DISABLED = -2,
    SRT_EXP_C_INVALID_INPUT = -3,
    SRT_EXP_C_OBJECT_NOT_FOUND = -4,
    SRT_EXP_C_MALLOC_FAILURE = -5,
    SRT_EXP_C_CONNECTION_FAILURE = -6,
} SrtExpCRet;


SrtExpCRet srtexp_c_init(const char *configFile);
SrtExpCRet srtexp_c_deinit();
SrtExpCRet srtexp_c_start(const char *exporterName, int *id);
SrtExpCRet srtexp_c_stop(const char *exporterName, int id);
SrtExpCRet srtexp_c_label_register(const char *name, const char *value, const char *var, int id);
SrtExpCRet srtexp_c_srt_socket_register(SRTSOCKET *sock, int sockNum, int id);
SrtExpCRet srtexp_c_set_log_dest(SrtExpCLogDestination dest);
SrtExpCRet srtexp_c_set_syslog_level(int level);

#ifdef __cplusplus
};
#endif
