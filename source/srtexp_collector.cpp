/*
 * SRT - Secure, Reliable, Transport
 * Copyright (c) 2022 Sony Group Corporation.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */

#include "include/srtexp_collector.hpp"

#include "include/srtexp_config.hpp"
#include "include/srtexp_logger.hpp"


namespace srt_exporter {

extern std::shared_ptr<SrtExpConfig> srtExpConfig;

// using namespace prometheus;

// SrtExpCollector
SrtExpCollector::SrtExpCollector(std::shared_ptr<SrtExporter> srtExp)
    : _srtExp(srtExp) {
    LoadFilterFromConfig();
}

SrtExpRet SrtExpCollector::LabelRegister(const char *name, const char *value,
                                         const char *var) {
    logger::SrtLog_Debug(__FUNCTION__);
    bool found = false;

    if (!name) {
        logger::SrtLog_Error("Null label name.");
        return SrtExpRet::SRT_EXP_INVALID_INPUT;
    }

    // If the input variable name is NULL, add the label to common label list.
    // If the input variable name is found in filtered entries, add the label
    // to the variable's private label list.
    std::vector<MetricLabel> *labelList = &(*_commonLabel);
    if (var) {
        logger::SrtLog_Notice("Allocating private label list. variable: "
                              + std::string(var));
        for (auto &data : (*_filteredData)) {
            if (data.name == var) {
                logger::SrtLog_Notice("Variable found.");
                found = true;
                labelList = &(data.label);
                break;
            }
        }
        if (found == false) {
            logger::SrtLog_Error("Variable not found.");
            return SrtExpRet::SRT_EXP_OBJECT_NOT_FOUND;
        }
    }

    found = false;
    for (auto label = labelList->begin(); label != labelList->end(); label++) {
        if (label->name == name) {
            logger::SrtLog_Notice("Existing label found. name: "
                                  + std::string(name));
            found = true;
            if (value) {
                logger::SrtLog_Notice("Update label value. value: "
                                      + std::string(value));
                label->value = std::string(value);
            } else {
                logger::SrtLog_Notice("Remove label.");
                labelList->erase(label);
            }
            break;
        }
    }
    if ((found == false) && (value)) {
        logger::SrtLog_Notice("Add new label. name: " + std::string(name)
                              + " value: " + std::string(value));
        labelList->push_back(
            MetricLabel({std::string(name), std::string(value)}));
    }

    return SrtExpRet::SRT_EXP_SUCCESS;
}

SrtExpRet SrtExpCollector::SrtSockRegister(SRTSOCKET *sock, int sockNum) {
    logger::SrtLog_Debug(__FUNCTION__);
    _srtSock->clear();

    if (sock) {
        for (int i = 0; i < sockNum; i++) {
            logger::SrtLog_Debug("srt socket adding: "
                                 + std::to_string(sock[i]));
            _srtSock->push_back(sock[i]);
        }
    }
    logger::SrtLog_Debug("srt socket num: "
                         + std::to_string(_srtSock->size()));

    return SrtExpRet::SRT_EXP_SUCCESS;
}

void SrtExpCollector::ValueToMetricFamily(
    const SrtDataMapper *map, MetricFamily *metric, SRTSOCKET sock) const {
    logger::SrtLog_Debug(__FUNCTION__);
    logger::SrtLog_Debug("Calculate number of labels.");
    int labelNum = 1 + map->label.size() + (*_commonLabel).size();

    logger::SrtLog_Debug("Convert value to double.");
    double valueDouble = 0;
    switch (map->structType) {
        case SrtDataValueType::SRT_INT:
            valueDouble = *reinterpret_cast<int *>(map->data);
            break;
        case SrtDataValueType::SRT_LONG_LONG:
            valueDouble = *reinterpret_cast<int64_t *>(map->data);
            break;
        case SrtDataValueType::SRT_UNSIGNED_LONG_LONG:
            valueDouble = *reinterpret_cast<uint64_t *>(map->data);
            break;
        case SrtDataValueType::SRT_DOUBLE:
        default:
            valueDouble = *reinterpret_cast<double *>(map->data);
    }

    logger::SrtLog_Debug("Fill metric.");
    metric->name = map->name;
    metric->help = map->help;
    metric->type = map->metricType;
    metric->metric.resize(1);
    metric->metric[0].label.resize(labelNum);
    metric->metric[0].label[0].name = "srt_socket";
    metric->metric[0].label[0].value = std::to_string(sock);
    for (int i = 0; i < map->label.size(); i++) {
        metric->metric[0].label[i + 1].name = map->label[i].name;
        metric->metric[0].label[i + 1].value = map->label[i].value;
    }
    for (int i = 0; i < (*_commonLabel).size(); i++) {
        metric->metric[0].label[i + map->label.size() + 1].name
            = (*_commonLabel)[i].name;
        metric->metric[0].label[i + map->label.size() + 1].value
            = (*_commonLabel)[i].value;
    }
    switch (metric->type) {
        case MetricType::Gauge:
            metric->metric[0].gauge.value = valueDouble;
            break;
        case MetricType::Counter:
            metric->metric[0].counter.value = valueDouble;
            break;
        case MetricType::Summary:
            break;
        case MetricType::Histogram:
            break;
        case MetricType::Untyped:
        default:
            metric->metric[0].untyped.value = valueDouble;
    }
}

void SrtExpCollector::LoadFilterFromConfig() {
    logger::SrtLog_Debug(__FUNCTION__);
    std::string exporterName = _srtExp->GetExporterName();
    SRT_TRACEBSTATS mold = {0};
    SrtDataMapper srtDataMapper[MAX_SRT_DATA_MAP_ENTRY_NUM]
        = SRT_DATA_MAP_INITIALIZER;
    SrtExpCollectorConfig config
        = srtExpConfig->GetSrtExpCollectorConfig(exporterName);

    for (int i = 0; i < MAX_SRT_DATA_MAP_ENTRY_NUM; i++) {
        if (srtDataMapper[i].name.empty()) {
            break;
        }
        for (auto &var : config.varList) {
            if (((config.filterMode == SrtExpFilterMode::WHITELIST)
                && (srtDataMapper[i].name == var))
                || ((config.filterMode != SrtExpFilterMode::WHITELIST)
                && (srtDataMapper[i].name != var))) {
                _filteredData->push_back(srtDataMapper[i]);
            }
        }
    }
    for (auto &label : config.labelList) {
        (*_commonLabel).push_back(MetricLabel({label.name, label.value}));
    }

    DumpFilter();
}

void SrtExpCollector::DumpFilter() {
    logger::SrtLog_Debug(__FUNCTION__);

    logger::SrtLog_Notice("Filtered data size: "
                          + std::to_string(_filteredData->size()));
    for (auto &data : (*_filteredData)) {
        logger::SrtLog_Debug("- name: " + data.name);
    }
}


// SrtExpCollectorModeOne
std::vector<MetricFamily> SrtExpCollectorModeOne::Collect() const {
    logger::SrtLog_Debug(__FUNCTION__);

    ClearTranslated();

    if (_srtSock->size()) {
        for (auto &sock : (*_srtSock)) {
            int ret;
            SRT_TRACEBSTATS stats;
            logger::SrtLog_Debug("calling srt_bstats: " + std::to_string(sock)
                + " from: " + std::to_string(pthread_self()));
            ret = srt_bstats(sock, &stats, 0);
            if (ret < 0) {
                continue;
            }
            DataAdaptor(&stats);
            MetricTranslation(sock);
        }
    }

    logger::SrtLog_Debug("Collect function returns.");
    return *_translated;
}

void SrtExpCollectorModeOne::DataAdaptor(SRT_TRACEBSTATS *stats) const {
    logger::SrtLog_Debug(__FUNCTION__);

    SrtDataMapper *map =  nullptr;
    for (auto &data : (*_filteredData)) {
        map = &data;
        switch (map->structType) {
            case SrtDataValueType::SRT_INT:
                *reinterpret_cast<int *>(map->data)
                    = *reinterpret_cast<int *>(
                        reinterpret_cast<char *>stats + map->offset);
                break;
            case SrtDataValueType::SRT_LONG_LONG:
                *reinterpret_cast<int64_t *>(map->data)
                    = *reinterpret_cast<int64_t *>(
                        reinterpret_cast<char *>stats + map->offset);
                break;
            case SrtDataValueType::SRT_UNSIGNED_LONG_LONG:
                *reinterpret_cast<uint64_t *>(map->data)
                    = *reinterpret_cast<uint64_t *>(
                        reinterpret_cast<char *>stats + map->offset);
                break;
            case SrtDataValueType::SRT_DOUBLE:
            default:
                *reinterpret_cast<double *>(map->data)
                    = *reinterpret_cast<double *>(
                        reinterpret_cast<char *>stats + map->offset);
        }
    }
}

void SrtExpCollectorModeOne::MetricTranslation(SRTSOCKET sock) const {
    logger::SrtLog_Debug(__FUNCTION__);

    SrtDataMapper *map = nullptr;
    int j = _translated->size();
    int offset = 0;
    for (auto &data : (*_filteredData)) {
        map = &data;
        _translated->resize(j + 1);
        ValueToMetricFamily((const SrtDataMapper *)map,
                            &(*_translated)[j], sock);
        j++;
        switch (map->structType) {
            case SrtDataValueType::SRT_INT:
                offset += sizeof(int);
                break;
            case SrtDataValueType::SRT_LONG_LONG:
                offset += sizeof(int64_t);
                break;
            case SrtDataValueType::SRT_UNSIGNED_LONG_LONG:
                offset += sizeof(uint64_t);
                break;
            case SrtDataValueType::SRT_DOUBLE:
            default:
                offset += sizeof(double);
        }
        logger::SrtLog_Debug(std::string(map->name) + " offset: "
                             + std::to_string(offset));
    }
}

}  // namespace srt_exporter
