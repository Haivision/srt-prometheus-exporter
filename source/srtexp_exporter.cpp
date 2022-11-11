#include "srtexp_exporter.hpp"

#include <stdexcept>

#include "srtexp_logger.hpp"

namespace srt_exporter {

extern std::shared_ptr<SrtExpConfig> srtExpConfig;

using namespace prometheus;

// SrtExporter
void SrtExporter::InitSrtExporter() {
    std::string ip = srtExpConfig->GetServerIp(_exporterName);
    int port = srtExpConfig->GetServerPort(_exporterName);
    std::string addr = ip + ":" + std::to_string(port);
    logger::SrtLog_Notice(_exporterName + "'s address: " + addr);
    try {
        _exposer = std::make_shared<Exposer>(addr);
    }
    catch (std::runtime_error &error) {
        logger::SrtLog_Error("Exposer failed to start http server.");
        throw SrtExpRet(SrtExpRet::SRT_EXP_FAILURE);
    }

    SrtExpCollectorMode collectorMode = srtExpConfig->GetCollectorMode(_exporterName);
    SetSrtExpCollector(collectorMode);

    _exposer->RegisterCollectable(_collector);
}

void SrtExporter::SetSrtExpCollector(SrtExpCollectorMode collectorMode) {
    if (_collector) {
        ResetSrtExpCollector();
    }

    switch (collectorMode) {
        case SrtExpCollectorMode::COLLECT_ON_REQUEST:
            _collector = std::make_shared<SrtExpCollectorModeOne>(shared_from_this());
            break;
        case SrtExpCollectorMode::COLLECT_PERIODICALLY:
        case SrtExpCollectorMode::RECEIVE_PASSIVELY:
        default:
            _collector = std::make_shared<SrtExpCollectorModeOne>(shared_from_this());
    }
}

} //namespace srt_exporter
