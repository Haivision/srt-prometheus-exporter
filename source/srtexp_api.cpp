/*
 * SRT - Secure, Reliable, Transport
 * Copyright (c) 2022 Sony Group Corporation.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */

#include "srtexp_api.hpp"

#include <cstdlib>
#include <cstring>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <vector>

#include "srtexp_exporter.hpp"
#include "srtexp_config.hpp"
#include "srtexp_logger.hpp"


namespace srt_exporter {

// global
std::vector<std::shared_ptr<SrtExporter>> srtExp = {};
std::shared_ptr<SrtExpConfig> srtExpConfig = nullptr;

}

using namespace srt_exporter;

// local
static std::mutex api_lock;

static std::shared_ptr<SrtExporter> FindSrtExporter(const char *exporterName, int *id);
static std::shared_ptr<SrtExporter> FindSrtExporter(int id);
static void DeleteSrtExporter(int id);

// api
SrtExpRet srtexp_init(const char *configFile) {
    logger::SrtLog_Notice(__FUNCTION__);

    srtExpConfig = std::make_shared<SrtExpConfig>();

    try {
        if (configFile) {
            srtExpConfig->LoadConfigFile(std::string(configFile));
        }
        else {
            srtExpConfig->LoadConfigFile(std::string(DEFAULT_CONFIG_FILE));
        }
    }
    catch (SrtExpRet &error) {
        logger::SrtLog_Error("Wrong with config file, using default config.");
        return error;
    }

    return SrtExpRet::SRT_EXP_SUCCESS;
}

SrtExpRet srtexp_deinit() {
    logger::SrtLog_Notice(__FUNCTION__);

    while (srtExp.size()) {
        *(srtExp.end()) = nullptr;
        srtExp.erase(srtExp.end());
    }
    srtExpConfig = nullptr;

    return SrtExpRet::SRT_EXP_SUCCESS;
}

SrtExpRet srtexp_start(const char *exporterName, int *id) {
    logger::SrtLog_Notice(__FUNCTION__);
    static int count = 0;
    std::lock_guard<std::mutex> lock(api_lock);
    count++;

    std::shared_ptr<SrtExporter> p = FindSrtExporter(exporterName, id);
    if (p) {
        srtexp_stop(exporterName);
    }

    srtExp.push_back(std::make_shared<SrtExporter>(std::string(exporterName), count));
    try {
        srtExp.back()->InitSrtExporter();
    }
    catch (SrtExpRet &error) {
        srtExp.erase(srtExp.end());
        return error;
    }

    *id = count;
    return SrtExpRet::SRT_EXP_SUCCESS;
}

SrtExpRet srtexp_stop(const char *exporterName) {
    logger::SrtLog_Notice(__FUNCTION__);
    int id = -1;
    std::lock_guard<std::mutex> lock(api_lock);

    std::shared_ptr<SrtExporter> p = FindSrtExporter(exporterName, &id);
    if (p) {
        DeleteSrtExporter(id);
        return SrtExpRet::SRT_EXP_SUCCESS;
    }
    else {
        return SrtExpRet::SRT_EXP_OBJECT_NOT_FOUND;
    }
}

SrtExpRet srtexp_stop(int id) {
    logger::SrtLog_Notice(__FUNCTION__);
    std::lock_guard<std::mutex> lock(api_lock);

    std::shared_ptr<SrtExporter> p = FindSrtExporter(id);
    if (p) {
        DeleteSrtExporter(id);
        return SrtExpRet::SRT_EXP_SUCCESS;
    }
    else {
        return SrtExpRet::SRT_EXP_OBJECT_NOT_FOUND;
    }
}

SrtExpRet srtexp_label_register(const char *name, const char *value, const char *var, int id) {
    logger::SrtLog_Notice(__FUNCTION__);
    std::lock_guard<std::mutex> lock(api_lock);

    std::shared_ptr<SrtExporter> p = FindSrtExporter(id);
    if (!p) {
        logger::SrtLog_Error("SrtExporter not found");
        return SrtExpRet::SRT_EXP_OBJECT_NOT_FOUND;
    }
    return p->GetSrtExpCollector()->LabelRegister(name, value, var);
}

SrtExpRet srtexp_srt_socket_register(SRTSOCKET *sock, int sockNum, int id) {
    logger::SrtLog_Notice(__FUNCTION__);
    std::lock_guard<std::mutex> lock(api_lock);

    std::shared_ptr<SrtExporter> p = FindSrtExporter(id);
    if (!p) {
        logger::SrtLog_Error("SrtExporter not found");
        return SrtExpRet::SRT_EXP_OBJECT_NOT_FOUND;
    }
    return p->GetSrtExpCollector()->SrtSockRegister(sock, sockNum);
}

SrtExpRet srtexp_set_log_dest(SrtExpLogDestination dest) {
    logger::SrtLog_Notice(__FUNCTION__);
    std::lock_guard<std::mutex> lock(api_lock);

    if (dest == SrtExpLogDestination::SRT_EXP_SYSLOG || dest == SrtExpLogDestination::SRT_EXP_STDERR) {
        logger::SrtLog_SetDestination(dest);

        return SrtExpRet::SRT_EXP_SUCCESS;
    }

    return SrtExpRet::SRT_EXP_INVALID_INPUT;
}

SrtExpRet srtexp_set_syslog_level(int level) {
    logger::SrtLog_Notice(__FUNCTION__);
    std::lock_guard<std::mutex> lock(api_lock);

    if (level <= LOG_DEBUG && level >= LOG_EMERG) {
        logger::SrtLog_SetSyslogLevel(level);

        return SrtExpRet::SRT_EXP_SUCCESS;
    }

    return SrtExpRet::SRT_EXP_INVALID_INPUT;
}

// utility
static std::shared_ptr<SrtExporter> FindSrtExporter(const char *exporterName, int *id) {
    if (exporterName) {
        for (std::shared_ptr<SrtExporter> p : srtExp) {
            if (p->CompareExporterName(exporterName)) {
                *id = p->GetId();
                return p;
            }
        }
    }
    return nullptr;
}

static std::shared_ptr<SrtExporter> FindSrtExporter(int id) {
    for (std::shared_ptr<SrtExporter> p : srtExp) {
        if (p->CompareId(id)) {
            return p;
        }
    }
    return nullptr;
}

static void DeleteSrtExporter(int id) {
    int pos = 0;
    for (std::shared_ptr<SrtExporter> p : srtExp) {
        if (p->CompareId(id)) {
            p->ResetSrtExpCollector();
            srtExp.erase(srtExp.begin() + pos);
            return;
        }
        pos++;
    }
}


