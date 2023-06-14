/*
 * SRT - Secure, Reliable, Transport
 * Copyright (c) 2022 Sony Group Corporation.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */

// head file for libsrtexp APIs

#pragma once

#include <srt/srt.h>

#include "export/srtexp_define.hpp"

/**
 * SRT Exporter library initialization, load configuration to memory
 *
 * @param   IN  configFile:     path to SRT Exporter configuration file, or nullptr to use default file
 * @return  SrtExpRet::SRT_EXP_SUCCESS on success
 */
SrtExpRet srtexp_init(const char *configFile);

/**
 * SRT Exporter library deinitialization, clear all SRT Exporter objects created, release loaded configuration
 *
 * @param
 * @return  SrtExpRet::SRT_EXP_SUCCESS on success
 */
SrtExpRet srtexp_deinit();

/**
 * Start an SRT Exporter object
 *
 * @param   IN  exporterName:   name string to identify the SRT Exporter object
 *          OUT id:             identity to allocate SRT Exporter object
 * @return  SrtExpRet::SRT_EXP_SUCCESS on success
 */
SrtExpRet srtexp_start(const char *exporterName, int *id);

/**
 * Stop an SRT Exporter object
 *
 * @param   IN  id:             identity to allocate SRT Exporter object
 * @return  SrtExpRet::SRT_EXP_SUCCESS on success
 */
SrtExpRet srtexp_stop(int id);

/**
 * Stop an SRT Exporter object
 *
 * @param   IN  exporterName:   name string to identify the SRT Exporter object
 * @return  SrtExpRet::SRT_EXP_SUCCESS on success
 */
SrtExpRet srtexp_stop(const char *exporterName);

/**
 * Add, change, or remove a label of one or all variables tracked by an SRT Exporter object
 *
 * @param   IN  name:           label key name
 *          IN  value:          label value, if nullptr, remove the existing label
 *          IN  var:            add label to which variable, if nullptr, add the label to all variables
 *          IN  id:             identity to allocate SRT Exporter object
 * @return  SrtExpRet::SRT_EXP_SUCCESS on success
 */
SrtExpRet srtexp_label_register(const char *name, const char *value,
                                const char *var, int id);

/**
 * Register SRT sockets to be tracked by one SRT Exporter object
 *
 * @param   IN  sock:           point to an array of SRTSOCKETs
 *          IN  sockNum:        number of input SRTSOCKETs
            IN  id:             identity to allocate SRT Exporter object
 * @return  SrtExpRet::SRT_EXP_SUCCESS on success
 */
SrtExpRet srtexp_srt_socket_register(SRTSOCKET *sock, int sockNum, int id);

/**
 * Set logging destination
 *
 * @param   IN dest:            trace output destination
 * @return  SrtExpRet::SRT_EXP_SUCCESS on success
 */
SrtExpRet srtexp_set_log_dest(SrtExpLogDestination dest);

/**
 * Set logging level
 *
 * @param   IN  level:          syslog debug level
 * @return  SrtExpRet::SRT_EXP_SUCCESS on success
 */
SrtExpRet srtexp_set_syslog_level(int level);

