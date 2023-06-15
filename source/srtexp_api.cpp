/*
 * SRT - Secure, Reliable, Transport
 * Copyright (c) 2022 Sony Group Corporation.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */

#include "export/srtexp_api.hpp"

#include <cstdlib>
#include <cstring>
#include <memory>
#include <mutex> // NOLINT
#include <stdexcept>
#include <vector>

#include "include/srtexp_exporter.hpp"
#include "include/srtexp_config.hpp"
#include "include/srtexp_logger.hpp"


// global
namespace srt_exporter {

std::vector<std::shared_ptr<SrtExporter>> srtExp = {};
std::shared_ptr<SrtExpConfig> srtExpConfig = nullptr;

}

// local
static std::mutex api_lock;

static std::shared_ptr<srt_exporter::SrtExporter> FindSrtExporter(
    const char *exporterName, int *id);
static std::shared_ptr<srt_exporter::SrtExporter> FindSrtExporter(int id);
static void DeleteSrtExporter(int id);

// api
SrtExpRet srtexp_init(const char *configFile) {
    srt_exporter::logger::SrtLog_Notice(__FUNCTION__);

    srt_exporter::srtExpConfig
        = std::make_shared<srt_exporter::SrtExpConfig>();

    try {
        if (configFile) {
            srt_exporter::srtExpConfig
                ->LoadConfigFile(std::string(configFile));
        } else {
            srt_exporter::srtExpConfig
                ->LoadConfigFile(std::string(DEFAULT_CONFIG_FILE));
        }
    }
    catch (SrtExpRet &error) {
        srt_exporter::logger::SrtLog_Error(
            "Wrong with config file, using default config.");
        return error;
    }

    return SrtExpRet::SRT_EXP_SUCCESS;
}

SrtExpRet srtexp_deinit() {
    srt_exporter::logger::SrtLog_Notice(__FUNCTION__);

    while (srt_exporter::srtExp.size()) {
        *(srt_exporter::srtExp.end()) = nullptr;
        srt_exporter::srtExp.erase(srt_exporter::srtExp.end());
    }
    srt_exporter::srtExpConfig = nullptr;

    return SrtExpRet::SRT_EXP_SUCCESS;
}

SrtExpRet srtexp_start(const char *exporterName, int *id) {
    srt_exporter::logger::SrtLog_Notice(__FUNCTION__);
    static int count = 0;
    std::lock_guard<std::mutex> lock(api_lock);
    count++;

    std::shared_ptr<srt_exporter::SrtExporter> p
        = FindSrtExporter(exporterName, id);
    if (p) {
        srtexp_stop(exporterName);
    }

    srt_exporter::srtExp.push_back(std::make_shared<srt_exporter::SrtExporter>(
        std::string(exporterName), count));
    try {
        srt_exporter::srtExp.back()->InitSrtExporter();
    }
    catch (SrtExpRet &error) {
        srt_exporter::srtExp.erase(srt_exporter::srtExp.end());
        return error;
    }

    *id = count;
    return SrtExpRet::SRT_EXP_SUCCESS;
}

SrtExpRet srtexp_stop(const char *exporterName) {
    srt_exporter::logger::SrtLog_Notice(__FUNCTION__);
    int id = -1;
    std::lock_guard<std::mutex> lock(api_lock);

    std::shared_ptr<srt_exporter::SrtExporter> p
        = FindSrtExporter(exporterName, &id);
    if (p) {
        DeleteSrtExporter(id);
        return SrtExpRet::SRT_EXP_SUCCESS;
    } else {
        return SrtExpRet::SRT_EXP_OBJECT_NOT_FOUND;
    }
}

SrtExpRet srtexp_stop(int id) {
    srt_exporter::logger::SrtLog_Notice(__FUNCTION__);
    std::lock_guard<std::mutex> lock(api_lock);

    std::shared_ptr<srt_exporter::SrtExporter> p = FindSrtExporter(id);
    if (p) {
        DeleteSrtExporter(id);
        return SrtExpRet::SRT_EXP_SUCCESS;
    } else {
        return SrtExpRet::SRT_EXP_OBJECT_NOT_FOUND;
    }
}

SrtExpRet srtexp_label_register(const char *name, const char *value,
                                const char *var, int id) {
    srt_exporter::logger::SrtLog_Notice(__FUNCTION__);
    std::lock_guard<std::mutex> lock(api_lock);

    std::shared_ptr<srt_exporter::SrtExporter> p = FindSrtExporter(id);
    if (!p) {
        srt_exporter::logger::SrtLog_Error("SrtExporter not found");
        return SrtExpRet::SRT_EXP_OBJECT_NOT_FOUND;
    }
    return p->GetSrtExpCollector()->LabelRegister(name, value, var);
}

SrtExpRet srtexp_srt_socket_register(SRTSOCKET *sock, int sockNum, int id) {
    srt_exporter::logger::SrtLog_Notice(__FUNCTION__);
    std::lock_guard<std::mutex> lock(api_lock);

    std::shared_ptr<srt_exporter::SrtExporter> p = FindSrtExporter(id);
    if (!p) {
        srt_exporter::logger::SrtLog_Error("SrtExporter not found");
        return SrtExpRet::SRT_EXP_OBJECT_NOT_FOUND;
    }
    return p->GetSrtExpCollector()->SrtSockRegister(sock, sockNum);
}

SrtExpRet srtexp_set_log_dest(SrtExpLogDestination dest) {
    srt_exporter::logger::SrtLog_Notice(__FUNCTION__);
    std::lock_guard<std::mutex> lock(api_lock);

    if (dest == SrtExpLogDestination::SRT_EXP_SYSLOG
        || dest == SrtExpLogDestination::SRT_EXP_STDERR) {
        srt_exporter::logger::SrtLog_SetDestination(dest);

        return SrtExpRet::SRT_EXP_SUCCESS;
    }

    return SrtExpRet::SRT_EXP_INVALID_INPUT;
}

SrtExpRet srtexp_set_syslog_level(int level) {
    srt_exporter::logger::SrtLog_Notice(__FUNCTION__);
    std::lock_guard<std::mutex> lock(api_lock);

    if (level <= LOG_DEBUG && level >= LOG_EMERG) {
        srt_exporter::logger::SrtLog_SetSyslogLevel(level);

        return SrtExpRet::SRT_EXP_SUCCESS;
    }

    return SrtExpRet::SRT_EXP_INVALID_INPUT;
}

// utility
static std::shared_ptr<srt_exporter::SrtExporter> FindSrtExporter(
    const char *exporterName, int *id) {
    if (exporterName) {
        for (std::shared_ptr<srt_exporter::SrtExporter> p
            : srt_exporter::srtExp) {
            if (p->CompareExporterName(exporterName)) {
                *id = p->GetId();
                return p;
            }
        }
    }
    return nullptr;
}

static std::shared_ptr<srt_exporter::SrtExporter> FindSrtExporter(int id) {
    for (std::shared_ptr<srt_exporter::SrtExporter> p : srt_exporter::srtExp) {
        if (p->CompareId(id)) {
            return p;
        }
    }
    return nullptr;
}

static void DeleteSrtExporter(int id) {
    int pos = 0;
    for (std::shared_ptr<srt_exporter::SrtExporter> p : srt_exporter::srtExp) {
        if (p->CompareId(id)) {
            p->ResetSrtExpCollector();
            srt_exporter::srtExp.erase(srt_exporter::srtExp.begin() + pos);
            return;
        }
        pos++;
    }
}
