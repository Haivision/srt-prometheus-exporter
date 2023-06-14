/*
 * SRT - Secure, Reliable, Transport
 * Copyright (c) 2022 Sony Group Corporation.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */

// head file for srt_exporter config object

#pragma once

#include <yaml-cpp/yaml.h>

#include <string>
#include <memory>
#include <vector>

#include "export/srtexp_define.hpp"

namespace srt_exporter {

enum class SrtExpCollectorMode{
    COLLECT_ON_REQUEST = 1,
    COLLECT_PERIODICALLY = 2,
    RECEIVE_PASSIVELY = 3,
};

enum class SrtExpFilterMode {
    WHITELIST = 1,
    BLACKLIST = 2,
    SRT_SOURCE = WHITELIST,
    SRT_DESTINATION = WHITELIST,
    SRT_COMMON = WHITELIST,
};

// Default static variable list for preset filter modes.
#define SRT_SOURCE_VARLIST "['pktSentTotal', 'pktSndLossTotal', 'pktSent', \
    'pktSndLoss', 'pktRetrans', 'pktRecvACK', 'pktRecvNAK', 'byteSent', \
    'byteRetrans', 'byteSndDrop', 'pktSndDrop', 'mbpsSendRate', \
    'usSndDuration', 'msSndTsbPdDelay', 'mbpsBandwidth', 'msRTT']"
#define SRT_DESTINATION_VARLIST "['pktRecvTotal', 'pktRcvLossTotal', \
    'pktRecv', 'pktRcvLoss', 'pktRcvRetrans', 'pktSentACK', 'pktSentNAK', \
    'byteRecv', 'byteRcvDrop', 'pktRcvDrop', 'mbpsRecvRate', 'usSndDuration', \
    'msSndTsbPdDelay', 'mbpsBandwidth', 'msRTT']"
#define SRT_COMMON_VARLIST "['pktSentTotal', 'pktSndLossTotal', 'pktSent', \
    'pktSndLoss', 'pktRetrans', 'pktRecvACK', 'pktRecvNAK', 'byteSent', \
    'byteRetrans', 'byteSndDrop', 'pktSndDrop', 'mbpsSendRate', \
    'pktRecvTotal', 'pktRcvLossTotal', 'pktRecv', 'pktRcvLoss', \
    'pktRcvRetrans', 'pktSentACK', 'pktSentNAK', 'byteRecv', 'byteRcvDrop', \
    'pktRcvDrop', 'mbpsRecvRate', 'usSndDuration', 'msSndTsbPdDelay', \
    'mbpsBandwidth', 'msRTT']"

typedef struct _SrtExpLabel {
    std::string name;
    std::string value;
} SrtExpLabel;

// Structure for collector configuration.
typedef struct _SrtExpCollectorConfig {
    SrtExpCollectorMode collectorMode;
    SrtExpFilterMode filterMode;
    std::vector<std::string> varList;
    std::vector<SrtExpLabel> labelList;
} SrtExpCollectorConfig;

// Structure for global configuration.
typedef struct _SrtExpGlobalConfig {
    std::string ip;
    int portMin;
    int portMax;    // a range of ports, from which available ports are
                    // assigned for new SRT Exporter objects
    SrtExpCollectorConfig config;
} SrtExpGlobalConfig;

typedef struct _SrtExpObjConfig {
    std::string name;
    std::string ip;
    int port;
    SrtExpCollectorConfig config;
} SrtExpObjConfig;


class SrtExpConfig {
 public:
    SrtExpConfig();

    // load the yaml-style configuraion file or default values into
    // _globalConfig and _objConfig
    void LoadConfigFile(const std::string& configFile);

    std::string GetServerIp(const std::string& exporterName);
    int GetServerPort(const std::string& exporterName);
    SrtExpCollectorMode GetCollectorMode(const std::string& exporterName);
    SrtExpCollectorConfig& GetSrtExpCollectorConfig(
        const std::string& exporterName);
    SrtExpObjConfig *FindSrtExpObjConfig(const std::string& exporterName);
    void DumpConfig();

 private:
    // global configuration for SRT Exporter object without specified
    // configuration
    SrtExpGlobalConfig _globalConfig;
    std::vector<SrtExpObjConfig> _objConfig;

    void LoadDefaultConfig(SrtExpGlobalConfig *cfg);
    void LoadDefaultSrtExpCollectorConfig(SrtExpCollectorConfig *cfg);
    void LoadSrtExpCollectorConfig(SrtExpCollectorConfig *cfg,
                                   const YAML::Node &config);
    void DumpSrtExpCollectorConfig(const SrtExpCollectorConfig &cfg);
    int IsPortAvailable(std::string ip, int port);
};

}  // namespace srt_exporter
