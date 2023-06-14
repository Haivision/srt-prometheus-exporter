/*
 * SRT - Secure, Reliable, Transport
 * Copyright (c) 2022 Sony Group Corporation.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */

#include "include/srtexp_config.hpp"

#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdexcept>

#include "include/srtexp_logger.hpp"


namespace srt_exporter {

// SrtExpConfig
SrtExpConfig::SrtExpConfig() {
    LoadDefaultConfig(&_globalConfig);
    _objConfig.clear();
}

void SrtExpConfig::LoadConfigFile(const std::string& configFile) {
    logger::SrtLog_Debug(__FUNCTION__);
    YAML::Node config;
    try {
        config = YAML::LoadFile(configFile);
    }
    catch (const YAML::BadFile &error) {
        logger::SrtLog_Error("Wrong file path.");
        throw SrtExpRet(SrtExpRet::SRT_EXP_INVALID_INPUT);
        return;
    }
    catch (const YAML::ParserException &error) {
        logger::SrtLog_Error("Parsing error.");
        throw SrtExpRet(SrtExpRet::SRT_EXP_FAILURE);
        return;
    }

    if (config["global"]) {
        if (config["global"]["ip"]) {
            _globalConfig.ip = config["global"]["ip"].as<std::string>();
        }
        if (config["global"]["port_min"] && config["global"]["port_max"]) {
            if (config["global"]["port_min"].as<int>()
                <= config["global"]["port_max"].as<int>()) {
                _globalConfig.portMin = config["global"]["port_min"].as<int>();
                _globalConfig.portMax = config["global"]["port_max"].as<int>();
            }
        }
        LoadSrtExpCollectorConfig(&(_globalConfig.config), config["global"]);
    }

    if (config["srt_exporters"]) {
        logger::SrtLog_Debug("srt_exporters: "
                             + std::to_string(config["srt_exporters"].size()));
        for (auto srtexp : config["srt_exporters"]) {
            if (srtexp["name"]) {
                if (FindSrtExpObjConfig(srtexp["name"].as<std::string>())) {
                    continue;
                }
                _objConfig.emplace_back(SrtExpObjConfig());
                _objConfig.back().name = srtexp["name"].as<std::string>();
                logger::SrtLog_Debug(_objConfig.back().name);
            } else {
                _objConfig.emplace_back(SrtExpObjConfig());
                _objConfig.back().name = "unnamed_srt_exporter_"
                                         + std::to_string(_objConfig.size());
            }
            if (srtexp["ip"]) {
                _objConfig.back().ip = srtexp["ip"].as<std::string>();
            }
            if (srtexp["port"]) {
                _objConfig.back().port = srtexp["port"].as<int>();
            }
            LoadDefaultSrtExpCollectorConfig(_objConfig.back().config);
            LoadSrtExpCollectorConfig(&(_objConfig.back().config), srtexp);
        }
    }

    DumpConfig();
}

std::string SrtExpConfig::GetServerIp(const std::string& exporterName) {
    SrtExpObjConfig *config = FindSrtExpObjConfig(exporterName);
    if (config && !(config->ip.empty())) {
        return config->ip;
    } else {
        return _globalConfig.ip;
    }
}

int SrtExpConfig::GetServerPort(const std::string& exporterName) {
    SrtExpObjConfig *config = FindSrtExpObjConfig(exporterName);
    if (config && (config->port)) {
        return config->port;
    } else {
        for (int i = _globalConfig.portMin; i <= _globalConfig.portMax; i++) {
            if (IsPortAvailable(_globalConfig.ip, i)) {
                return i;
            }
        }
        return -1;
    }
}

SrtExpCollectorMode SrtExpConfig::GetCollectorMode(
    const std::string& exporterName) {
    SrtExpObjConfig *config = FindSrtExpObjConfig(exporterName);
    if (config) {
        return config->config.collectorMode;
    } else {
        return _globalConfig.config.collectorMode;
    }
}

SrtExpCollectorConfig& SrtExpConfig::GetSrtExpCollectorConfig(
    const std::string& exporterName) {
    SrtExpObjConfig *config = FindSrtExpObjConfig(exporterName);
    if (config) {
        return config->config;
    } else {
        return _globalConfig.config;
    }
}

SrtExpObjConfig *SrtExpConfig::FindSrtExpObjConfig(
    const std::string& exporterName) {
    logger::SrtLog_Debug(__FUNCTION__);
    logger::SrtLog_Debug(exporterName);
    for (auto &cfg : _objConfig) {
        logger::SrtLog_Debug(cfg.name);
        if (exporterName == cfg.name) {
            return &cfg;
        }
    }
    logger::SrtLog_Notice("Srt Exporter object not found!");
    return nullptr;
}

void SrtExpConfig::DumpConfig() {
    logger::SrtLog_Debug(__FUNCTION__);

    logger::SrtLog_Debug("global:");
    logger::SrtLog_Debug("  ip: " + _globalConfig.ip + "     port: "
                         + std::to_string(_globalConfig.portMin) + "~"
                         + std::to_string(_globalConfig.portMax));
    DumpSrtExpCollectorConfig(_globalConfig.config);

    logger::SrtLog_Debug("srt_exporters:");
    for (auto &cfg : _objConfig) {
        logger::SrtLog_Debug("- name: " + cfg.name);
        logger::SrtLog_Debug("  ip: " + cfg.ip + "     port: "
                             + std::to_string(cfg.port));
        DumpSrtExpCollectorConfig(cfg.config);
    }
}

void SrtExpConfig::LoadDefaultConfig(SrtExpGlobalConfig *cfg) {
    cfg->ip = std::string("0.0.0.0");
    cfg->portMin = 9901;
    cfg->portMax = 10028;
    LoadDefaultSrtExpCollectorConfig(&(cfg->config));
}

void SrtExpConfig::LoadDefaultSrtExpCollectorConfig(
    SrtExpCollectorConfig *cfg) {
    cfg->collectorMode = SrtExpCollectorMode::COLLECT_ON_REQUEST;
    cfg->filterMode = SrtExpFilterMode::SRT_COMMON;
    YAML::Node temp = YAML::Load(SRT_COMMON_VARLIST);
    cfg->varList.clear();
    for (auto tmp : temp) {
        cfg->varList.push_back(tmp.as<std::string>());
    }
    cfg->labelList.clear();
}

void SrtExpConfig::LoadSrtExpCollectorConfig(SrtExpCollectorConfig *cfg,
                                             const YAML::Node &config) {
    if (config["collector_mode"]) {
        std::string str = config["collector_mode"].as<std::string>();
        if (str == "collect_on_request") {
            cfg->collectorMode = SrtExpCollectorMode::COLLECT_ON_REQUEST;
        } else {
            logger::SrtLog_Error("Method not implemented yet.");
            cfg->collectorMode = SrtExpCollectorMode::COLLECT_ON_REQUEST;
        }
    }

    if (config["filter"]) {
        logger::SrtLog_Debug("Filter block found.");
        if (config["filter"]["filter_mode"]) {
            cfg->varList.clear();
            std::string str
                = config["filter"]["filter_mode"].as<std::string>();
            logger::SrtLog_Debug(str);
            if (str == "whitelist") {
                cfg->filterMode = SrtExpFilterMode::WHITELIST;
                if (config["filter"]["whitelist"]) {
                    for (auto var : config["filter"]["whitelist"]) {
                        cfg->varList.push_back(var.as<std::string>());
                    }
                }
            } else if (str == "blacklist") {
                cfg->filterMode = SrtExpFilterMode::BLACKLIST;
                if (config["filter"]["blacklist"]) {
                    for (auto var : config["filter"]["blacklist"]) {
                        cfg->varList.push_back(var.as<std::string>());
                    }
                }
            } else if (str == "srt_source") {
                cfg->filterMode = SrtExpFilterMode::SRT_SOURCE;
                YAML::Node temp = YAML::Load(SRT_SOURCE_VARLIST);
                for (auto tmp : temp) {
                    cfg->varList.push_back(tmp.as<std::string>());
                }
            } else if (str == "srt_destination") {
                cfg->filterMode = SrtExpFilterMode::SRT_DESTINATION;
                YAML::Node temp = YAML::Load(SRT_DESTINATION_VARLIST);
                for (auto tmp : temp) {
                    cfg->varList.push_back(tmp.as<std::string>());
                }
            }
        }
    }

    if (config["labels"]) {
        cfg->labelList.clear();
        for (auto label : config["labels"]) {
            cfg->labelList.emplace_back(
                SrtExpLabel({label["name"].as<std::string>(),
                             label["value"].as<std::string>()}));
        }
    }
}

void SrtExpConfig::DumpSrtExpCollectorConfig(
    const SrtExpCollectorConfig &cfg) {
    logger::SrtLog_Debug("  collector_mode: "
        + std::to_string(static_cast<int>(cfg.collectorMode))
        + "     filter_mode: "
        + std::to_string(static_cast<int>(cfg.filterMode)));
    for (auto &var : cfg.varList) {
        logger::SrtLog_Debug("  - " + var);
    }
    logger::SrtLog_Debug("  labels: ");
    for (auto &label : cfg.labelList) {
        logger::SrtLog_Debug("  - name: " + label.name
                             + "     value: " + label.value);
    }
}

int SrtExpConfig::IsPortAvailable(std::string ip, int port) {
    struct sockaddr_in addr;
    int ret = 0;

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        return 0;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    ret = inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);
    if (ret == 0 || ret == -1) {
        logger::SrtLog_Error("Wrong ip address input or available address
                             family not found.");
        close(fd);
        return 0;
    }

    ret = bind(fd, (struct sockaddr *)(&addr), sizeof(struct sockaddr_in));
    close(fd);
    if (ret) {
        return 0;
    } else {
        return 1;
    }
}

}  // namespace srt_exporter
