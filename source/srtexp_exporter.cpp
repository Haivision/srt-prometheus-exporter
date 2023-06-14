/*
 * SRT - Secure, Reliable, Transport
 * Copyright (c) 2022 Sony Group Corporation.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */

#include "include/srtexp_exporter.hpp"

#include <stdexcept>

#include "include/srtexp_logger.hpp"


namespace srt_exporter {

extern std::shared_ptr<SrtExpConfig> srtExpConfig;

// SrtExporter
void SrtExporter::InitSrtExporter() {
    std::string ip = srtExpConfig->GetServerIp(_exporterName);
    int port = srtExpConfig->GetServerPort(_exporterName);
    std::string addr = ip + ":" + std::to_string(port);
    logger::SrtLog_Notice(_exporterName + "'s address: " + addr);
    try {
        _exposer = std::make_shared<prometheus::Exposer>(addr);
    }
    catch (std::runtime_error &error) {
        logger::SrtLog_Error("Exposer failed to start http server.");
        throw SrtExpRet(SrtExpRet::SRT_EXP_FAILURE);
    }

    SrtExpCollectorMode collectorMode
        = srtExpConfig->GetCollectorMode(_exporterName);
    SetSrtExpCollector(collectorMode);

    _exposer->RegisterCollectable(_collector);
}

void SrtExporter::SetSrtExpCollector(SrtExpCollectorMode collectorMode) {
    if (_collector) {
        ResetSrtExpCollector();
    }

    switch (collectorMode) {
        case SrtExpCollectorMode::COLLECT_ON_REQUEST:
            _collector = std::make_shared<SrtExpCollectorModeOne>(
                shared_from_this());
            break;
        case SrtExpCollectorMode::COLLECT_PERIODICALLY:
        case SrtExpCollectorMode::RECEIVE_PASSIVELY:
        default:
            _collector = std::make_shared<SrtExpCollectorModeOne>(
                shared_from_this());
    }
}

}  // namespace srt_exporter
