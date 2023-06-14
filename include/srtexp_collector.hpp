/*
 * SRT - Secure, Reliable, Transport
 * Copyright (c) 2022 Sony Group Corporation.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */

// head file for srt_exporter collectable collector

#pragma once

#include <srt/srt.h>
#include <prometheus/collectable.h>
#include <prometheus/metric_family.h>

#include <string>
#include <vector>
#include <memory>

#include "export/srtexp_define.hpp"
#include "include/srtexp_exporter.hpp"


namespace srt_exporter {

// using namespace prometheus;
class SrtExporter;

#define STAT_DATA_BUFF_LEN 64
#define MAX_SRT_DATA_MAP_ENTRY_NUM 128

// All possible SrtDataMapper entries based on "struct CBytePerfMon".
#define SRT_DATA_MAP_INITIALIZER {\
{"msTimeStamp",             "time since the UDT entity is started, in milliseconds",            SrtDataValueType::SRT_LONG_LONG,            MetricType::Counter,    {},     static_cast<size_t>(&(mold.msTimeStamp)) - static_cast<size_t>(&mold),               0}, \
{"pktSentTotal",            "total number of sent data packets, including retransmissions",     SrtDataValueType::SRT_LONG_LONG,            MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktSentTotal)) - static_cast<size_t>(&mold),              0}, \
{"pktRecvTotal",            "total number of received packets",                                 SrtDataValueType::SRT_LONG_LONG,            MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktRecvTotal)) - static_cast<size_t>(&mold),              0}, \
{"pktSndLossTotal",         "total number of lost packets (sender side)",                       SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktSndLossTotal)) - static_cast<size_t>(&mold),           0}, \
{"pktRcvLossTotal",         "total number of lost packets (receiver side)",                     SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktRcvLossTotal)) - static_cast<size_t>(&mold),           0}, \
{"pktRetransTotal",         "total number of retransmitted packets",                            SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktRetransTotal)) - static_cast<size_t>(&mold),           0}, \
{"pktSentACKTotal",         "total number of sent ACK packets",                                 SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktSentACKTotal)) - static_cast<size_t>(&mold),           0}, \
{"pktRecvACKTotal",         "total number of received ACK packets",                             SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktRecvACKTotal)) - static_cast<size_t>(&mold),           0}, \
{"pktSentNAKTotal",         "total number of sent NAK packets",                                 SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktSentNAKTotal)) - static_cast<size_t>(&mold),           0}, \
{"pktRecvNAKTotal",         "total number of received NAK packets",                             SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktRecvACKTotal)) - static_cast<size_t>(&mold),           0}, \
{"usSndDurationTotal",      "total time duration when UDT is sending data (no idle time)",      SrtDataValueType::SRT_LONG_LONG,            MetricType::Counter,    {},     static_cast<size_t>(&(mold.usSndDurationTotal)) - static_cast<size_t>(&mold),        0}, \
{"pktSndDropTotal",         "number of too-late-to-send dropped packets",                       SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktSndDropTotal)) - static_cast<size_t>(&mold),           0}, \
{"pktRcvDropTotal",         "number of too-late-to play missing packets",                       SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktRcvDropTotal)) - static_cast<size_t>(&mold),           0}, \
{"pktRcvUndecryptTotal",    "number of undecrypted packets",                                    SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktRcvUndecryptTotal)) - static_cast<size_t>(&mold),      0}, \
{"byteSentTotal",           "total number of sent data bytes, including retransmissions",       SrtDataValueType::SRT_UNSIGNED_LONG_LONG,   MetricType::Counter,    {},     static_cast<size_t>(&(mold.byteSentTotal)) - static_cast<size_t>(&mold),             0}, \
{"byteRecvTotal",           "total number of received bytes",                                   SrtDataValueType::SRT_UNSIGNED_LONG_LONG,   MetricType::Counter,    {},     static_cast<size_t>(&(mold.byteRecvTotal)) - static_cast<size_t>(&mold),             0}, \
{"byteRcvLossTotal",        "total number of lost bytes",                                       SrtDataValueType::SRT_UNSIGNED_LONG_LONG,   MetricType::Counter,    {},     static_cast<size_t>(&(mold.byteRcvLossTotal)) - static_cast<size_t>(&mold),          0}, \
{"byteRetransTotal",        "total number of retransmitted bytes",                              SrtDataValueType::SRT_UNSIGNED_LONG_LONG,   MetricType::Counter,    {},     static_cast<size_t>(&(mold.byteRetransTotal)) - static_cast<size_t>(&mold),          0}, \
{"byteSndDropTotal",        "number of too-late-to-send dropped bytes",                         SrtDataValueType::SRT_UNSIGNED_LONG_LONG,   MetricType::Counter,    {},     static_cast<size_t>(&(mold.byteSndDropTotal)) - static_cast<size_t>(&mold),          0}, \
{"byteRcvDropTotal",        "number of too-late-to play missing bytes (estimation)",            SrtDataValueType::SRT_UNSIGNED_LONG_LONG,   MetricType::Counter,    {},     static_cast<size_t>(&(mold.byteRcvDropTotal)) - static_cast<size_t>(&mold),          0}, \
{"byteRcvUndecryptTotal",   "number of undecrypted bytes",                                      SrtDataValueType::SRT_UNSIGNED_LONG_LONG,   MetricType::Counter,    {},     static_cast<size_t>(&(mold.byteRcvUndecryptTotal)) - static_cast<size_t>(&mold),     0}, \
{"pktSent",                 "number of sent data packets, including retransmissions",           SrtDataValueType::SRT_LONG_LONG,            MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktSent)) - static_cast<size_t>(&mold),                   0}, \
{"pktRecv",                 "number of received packets",                                       SrtDataValueType::SRT_LONG_LONG,            MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktRecv)) - static_cast<size_t>(&mold),                   0}, \
{"pktSndLoss",              "number of lost packets (sender side)",                             SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktSndLoss)) - static_cast<size_t>(&mold),                0}, \
{"pktRcvLoss",              "number of lost packets (receiver side)",                           SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktRcvLoss)) - static_cast<size_t>(&mold),                0}, \
{"pktRetrans",              "number of retransmitted packets",                                  SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktRetrans)) - static_cast<size_t>(&mold),                0}, \
{"pktRcvRetrans",           "number of retransmitted packets received",                         SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktRcvRetrans)) - static_cast<size_t>(&mold),             0}, \
{"pktSentACK",              "number of sent ACK packets",                                       SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktSentACK)) - static_cast<size_t>(&mold),                0}, \
{"pktRecvACK",              "number of received ACK packets",                                   SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktRecvACK)) - static_cast<size_t>(&mold),                0}, \
{"pktSentNAK",              "number of sent NAK packets",                                       SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktSentNAK)) - static_cast<size_t>(&mold),                0}, \
{"pktRecvNAK",              "number of received NAK packets",                                   SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktRecvNAK)) - static_cast<size_t>(&mold),                0}, \
{"mbpsSendRate",            "sending rate in Mb/s",                                             SrtDataValueType::SRT_DOUBLE,               MetricType::Gauge,      {},     static_cast<size_t>(&(mold.mbpsSendRate)) - static_cast<size_t>(&mold),              0}, \
{"mbpsRecvRate",            "receiving rate in Mb/s",                                           SrtDataValueType::SRT_DOUBLE,               MetricType::Gauge,      {},     static_cast<size_t>(&(mold.mbpsRecvRate)) - static_cast<size_t>(&mold),              0}, \
{"usSndDuration",           "busy sending time (i.e., idle time exclusive)",                    SrtDataValueType::SRT_LONG_LONG,            MetricType::Counter,    {},     static_cast<size_t>(&(mold.usSndDuration)) - static_cast<size_t>(&mold),             0}, \
{"pktReorderDistance",      "size of order discrepancy in received sequences",                  SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktReorderDistance)) - static_cast<size_t>(&mold),        0}, \
{"pktRcvAvgBelatedTime",    "average time of packet delay for belated packets",                 SrtDataValueType::SRT_DOUBLE,               MetricType::Gauge,      {},     static_cast<size_t>(&(mold.pktRcvAvgBelatedTime)) - static_cast<size_t>(&mold),      0}, \
{"pktRcvBelated",           "number of received AND IGNORED packets for coming too late",       SrtDataValueType::SRT_LONG_LONG,            MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktRcvBelated)) - static_cast<size_t>(&mold),             0}, \
{"pktSndDrop",              "number of too-late-to-send dropped packets",                       SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktSndDrop)) - static_cast<size_t>(&mold),                0}, \
{"pktRcvDrop",              "number of too-late-to play missing packets",                       SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktRcvDrop)) - static_cast<size_t>(&mold),                0}, \
{"pktRcvUndecrypt",         "number of undecrypted packets",                                    SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktRcvUndecrypt)) - static_cast<size_t>(&mold),           0}, \
{"byteSent",                "number of sent data bytes, including retransmissions",             SrtDataValueType::SRT_UNSIGNED_LONG_LONG,   MetricType::Counter,    {},     static_cast<size_t>(&(mold.byteSent)) - static_cast<size_t>(&mold),                  0}, \
{"byteRecv",                "number of received bytes",                                         SrtDataValueType::SRT_UNSIGNED_LONG_LONG,   MetricType::Counter,    {},     static_cast<size_t>(&(mold.byteRecv)) - static_cast<size_t>(&mold),                  0}, \
{"byteRcvLoss",             "number of retransmitted bytes",                                    SrtDataValueType::SRT_UNSIGNED_LONG_LONG,   MetricType::Counter,    {},     static_cast<size_t>(&(mold.byteRcvLoss)) - static_cast<size_t>(&mold),               0}, \
{"byteRetrans",             "number of retransmitted bytes",                                    SrtDataValueType::SRT_UNSIGNED_LONG_LONG,   MetricType::Counter,    {},     static_cast<size_t>(&(mold.byteRetrans)) - static_cast<size_t>(&mold),               0}, \
{"byteSndDrop",             "number of too-late-to-send dropped bytes",                         SrtDataValueType::SRT_UNSIGNED_LONG_LONG,   MetricType::Counter,    {},     static_cast<size_t>(&(mold.byteSndDrop)) - static_cast<size_t>(&mold),               0}, \
{"byteRcvDrop",             "number of too-late-to play missing bytes (estimation)",            SrtDataValueType::SRT_UNSIGNED_LONG_LONG,   MetricType::Counter,    {},     static_cast<size_t>(&(mold.byteRcvDrop)) - static_cast<size_t>(&mold),               0}, \
{"byteRcvUndecrypt",        "number of undecrypted bytes",                                      SrtDataValueType::SRT_UNSIGNED_LONG_LONG,   MetricType::Counter,    {},     static_cast<size_t>(&(mold.byteRcvUndecrypt)) - static_cast<size_t>(&mold),          0}, \
{"usPktSndPeriod",          "packet sending period, in microseconds",                           SrtDataValueType::SRT_DOUBLE,               MetricType::Gauge,      {},     static_cast<size_t>(&(mold.usPktSndPeriod)) - static_cast<size_t>(&mold),            0}, \
{"pktFlowWindow",           "flow window size, in number of packets",                           SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktFlowWindow)) - static_cast<size_t>(&mold),             0}, \
{"pktCongestionWindow",     "congestion window size, in number of packets",                     SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktCongestionWindow)) - static_cast<size_t>(&mold),       0}, \
{"pktFlightSize",           "number of packets on flight",                                      SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktFlightSize)) - static_cast<size_t>(&mold),             0}, \
{"msRTT",                   "RTT, in milliseconds",                                             SrtDataValueType::SRT_DOUBLE,               MetricType::Gauge,      {},     static_cast<size_t>(&(mold.msRTT)) - static_cast<size_t>(&mold),                     0}, \
{"mbpsBandwidth",           "estimated bandwidth, in Mb/s",                                     SrtDataValueType::SRT_DOUBLE,               MetricType::Gauge,      {},     static_cast<size_t>(&(mold.mbpsBandwidth)) - static_cast<size_t>(&mold),             0}, \
{"byteAvailSndBuf",         "available UDT sender buffer size",                                 SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.byteAvailSndBuf)) - static_cast<size_t>(&mold),           0}, \
{"byteAvailRcvBuf",         "available UDT receiver buffer size",                               SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.byteAvailRcvBuf)) - static_cast<size_t>(&mold),           0}, \
{"mbpsMaxBW",               "Transmit Bandwidth ceiling (Mbps)",                                SrtDataValueType::SRT_DOUBLE,               MetricType::Gauge,      {},     static_cast<size_t>(&(mold.mbpsMaxBW)) - static_cast<size_t>(&mold),                 0}, \
{"byteMSS",                 "MTU",                                                              SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.byteMSS)) - static_cast<size_t>(&mold),                   0}, \
{"pktSndBuf",               "UnACKed packets in UDT sender",                                    SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktSndBuf)) - static_cast<size_t>(&mold),                 0}, \
{"byteSndBuf",              "UnACKed bytes in UDT sender",                                      SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.byteSndBuf)) - static_cast<size_t>(&mold),                0}, \
{"msSndBuf",                "UnACKed timespan (msec) of UDT sender",                            SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.msSndBuf)) - static_cast<size_t>(&mold),                  0}, \
{"msSndTsbPdDelay",         "Timestamp-based Packet Delivery Delay",                            SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.msSndTsbPdDelay)) - static_cast<size_t>(&mold),           0}, \
{"pktRcvBuf",               "Undelivered packets in UDT receiver",                              SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktRcvBuf)) - static_cast<size_t>(&mold),                 0}, \
{"byteRcvBuf",              "Undelivered bytes of UDT receiver",                                SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.byteRcvBuf)) - static_cast<size_t>(&mold),                0}, \
{"msRcvBuf",                "Undelivered timespan (msec) of UDT receiver",                      SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.msRcvBuf)) - static_cast<size_t>(&mold),                  0}, \
{"msRcvTsbPdDelay",         "Timestamp-based Packet Delivery Delay",                            SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.msRcvTsbPdDelay)) - static_cast<size_t>(&mold),           0}, \
{"pktSndFilterExtraTotal",  "number of control packets supplied by packet filter",              SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktSndFilterExtraTotal)) - static_cast<size_t>(&mold),    0}, \
{"pktRcvFilterExtraTotal",  "number of control packets received and not supplied back",         SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktRcvFilterExtraTotal)) - static_cast<size_t>(&mold),    0}, \
{"pktRcvFilterSupplyTotal", "number of packets that the filter supplied extra",                 SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktRcvFilterSupplyTotal)) - static_cast<size_t>(&mold),   0}, \
{"pktRcvFilterLossTotal",   "number of packet loss not coverable by filter",                    SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktRcvFilterLossTotal)) - static_cast<size_t>(&mold),     0}, \
{"pktSndFilterExtra",       "number of control packets supplied by packet filter",              SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktSndFilterExtra)) - static_cast<size_t>(&mold),         0}, \
{"pktRcvFilterExtra",       "number of control packets received and not supplied back",         SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktRcvFilterExtra)) - static_cast<size_t>(&mold),         0}, \
{"pktRcvFilterSupply",      "number of packets that the filter supplied extra",                 SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktRcvFilterSupply)) - static_cast<size_t>(&mold),        0}, \
{"pktRcvFilterLoss",        "number of packet loss not coverable by filter",                    SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktRcvFilterLoss)) - static_cast<size_t>(&mold),          0}, \
}

// New items in libsrt v1.5.0
/* 
{"pktReorderTolerance",     "packet reorder tolerance value",                                   SrtDataValueType::SRT_INT,                  MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktReorderTolerance)) - static_cast<size_t>(&mold),       0}, \
{"pktSentUniqueTotal",      "total number of data packets sent by the application",             SrtDataValueType::SRT_LONG_LONG,            MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktSentUniqueTotal)) - static_cast<size_t>(&mold),        0}, \
{"pktRecvUniqueTotal",      "total number of packets to be received by the application",        SrtDataValueType::SRT_LONG_LONG,            MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktRecvUniqueTotal)) - static_cast<size_t>(&mold),        0}, \
{"byteSentUniqueTotal",     "total number of data bytes, sent by the application",              SrtDataValueType::SRT_UNSIGNED_LONG_LONG,   MetricType::Counter,    {},     static_cast<size_t>(&(mold.byteSentUniqueTotal)) - static_cast<size_t>(&mold),       0}, \
{"byteRecvUniqueTotal",     "total number of data bytes to be received by the application",     SrtDataValueType::SRT_UNSIGNED_LONG_LONG,   MetricType::Counter,    {},     static_cast<size_t>(&(mold.byteRecvUniqueTotal)) - static_cast<size_t>(&mold),       0}, \
{"pktSentUnique",           "number of data packets sent by the application",                   SrtDataValueType::SRT_LONG_LONG,            MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktSentUnique)) - static_cast<size_t>(&mold),             0}, \
{"pktRecvUnique",           "number of packets to be received by the application",              SrtDataValueType::SRT_LONG_LONG,            MetricType::Counter,    {},     static_cast<size_t>(&(mold.pktRecvUnique)) - static_cast<size_t>(&mold),             0}, \
{"byteSentUnique",          "number of data bytes, sent by the application",                    SrtDataValueType::SRT_UNSIGNED_LONG_LONG,   MetricType::Counter,    {},     static_cast<size_t>(&(mold.byteSentUnique)) - static_cast<size_t>(&mold),            0}, \
{"byteRecvUnique",          "number of data bytes to be received by the application",           SrtDataValueType::SRT_UNSIGNED_LONG_LONG,   MetricType::Counter,    {},     static_cast<size_t>(&(mold.byteRecvUnique)) - static_cast<size_t>(&mold),            0}, \
*/

enum class SrtDataValueType {
    SRT_INT,
    SRT_LONG_LONG,
    SRT_UNSIGNED_LONG_LONG,
    SRT_DOUBLE,
};

typedef struct _MetricLabel {
    std::string name;
    std::string value;
} MetricLabel;

// Structure for a single filter entry.
typedef struct _SrtDataMapper {
    std::string name;                   // the name of the variable
    std::string help;                   // a description
    SrtDataValueType structType;        // data types for convertion
    MetricType metricType;              // metric type after convertion
    std::vector<MetricLabel> label;     // label list
                                        // (special labels not registered
                                        // as common labels are stored here)
    size_t offset;                      // offset in "struct CBytePerfMon"
    char data[STAT_DATA_BUFF_LEN];      // a buffer for storing the value
} SrtDataMapper;


class SrtExpCollector : public Collectable {
 public:
    explicit SrtExpCollector(std::shared_ptr<SrtExporter> srtExp);

    SrtExpRet LabelRegister(const char *name, const char *value,
                            const char *var);
    SrtExpRet SrtSockRegister(SRTSOCKET *sock, int sockNum);

 protected:
    void ClearTranslated() const {_translated->clear();}
    // translate SRT data to metrics to be sent
    void ValueToMetricFamily(const SrtDataMapper *map, MetricFamily *metric,
                             SRTSOCKET sock) const;
    void LoadFilterFromConfig();
    void DumpFilter();

    // the SRT Exporter object contains this SRT Exporter collector object
    std::shared_ptr<SrtExporter> _srtExp;
    // a list of labels for all entries in the metrics to be sent
    std::shared_ptr<std::vector<MetricLabel>> _commonLabel
        = std::make_shared<std::vector<MetricLabel>>();
    // a list of registered SRT sockets tracked by this collector
    std::shared_ptr<std::vector<SRTSOCKET>> _srtSock
        = std::make_shared<std::vector<SRTSOCKET>>();
    // translated SRT data, ready to be collected as metrics
    std::shared_ptr<std::vector<MetricFamily>> _translated
        = std::make_shared<std::vector<MetricFamily>>();
    // filtered entries according to cofiguration
    std::shared_ptr<std::vector<SrtDataMapper>> _filteredData
        = std::make_shared<std::vector<SrtDataMapper>>();
};

class SrtExpCollectorModeOne : public SrtExpCollector {
// mode 1: collect on request
 public:
    using SrtExpCollector::SrtExpCollector;
    std::vector<MetricFamily> Collect() const override;

 private:
    // convert data from SRT library API to local format
    void DataAdaptor(SRT_TRACEBSTATS *stats) const;
    // call `ValueToMetricFamily` for each variable
    void MetricTranslation(SRTSOCKET sock) const;
};

}  // namespace srt_exporter
