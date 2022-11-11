// head file for srt_exporter core object

#pragma once

#include <string>
#include <memory>

#include <prometheus/exposer.h>

#include "srtexp_define.hpp"
#include "srtexp_collector.hpp"
#include "srtexp_config.hpp"


namespace srt_exporter {

using namespace prometheus;

class SrtExpCollector;

class SrtExporter : public std::enable_shared_from_this<SrtExporter>{
public:
    SrtExporter(std::string exporterName, int id) : _exporterName(exporterName), _id(id) {};
    // create the exposer object and the collector object according to configuration
    void InitSrtExporter();

    std::shared_ptr<Exposer> GetSrtExposer() {return _exposer;}
    std::shared_ptr<SrtExpCollector> GetSrtExpCollector() {return _collector;}
    void ResetSrtExpCollector() {_collector = nullptr;}
    void SetSrtExpCollector(SrtExpCollectorMode collectorMode);

    bool CompareExporterName(const char *exporterName) {return (std::string(exporterName) == _exporterName);}
    bool CompareId(int id) {return (id == _id);}
    std::string GetExporterName() {return _exporterName;}
    int GetId() {return _id;}

private:
    // the unique name of the SRT Exporter object in string type
    std::string _exporterName;
    // unique integer identity of the SRT Exporter object generated when starting SRT Exporter object API is called
    int _id;
    // object implemented by Prometheus c++ client library
    // providing the http server for Prometheus to request metrics from, and an interface to register collector function
    std::shared_ptr<Exposer> _exposer;
    // SRT Exporter collector object of this SRT Exporter object
    std::shared_ptr<SrtExpCollector> _collector;
};

} //namespace srt_exporter
