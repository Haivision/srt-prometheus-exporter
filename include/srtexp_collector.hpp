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

class SrtExporter;

#define STAT_DATA_BUFF_LEN 64
#define MAX_SRT_DATA_MAP_ENTRY_NUM 128

// All possible SrtDataMapper entries based on "struct CBytePerfMon".
#define SRT_DATA_MAP_INITIALIZER {\
{"msTimeStamp",             "time since the UDT entity is started, in milliseconds",            SrtDataValueType::SRT_LONG_LONG,            prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.msTimeStamp)) - reinterpret_cast<uintptr_t>(&mold),              0}, \
{"pktSentTotal",            "total number of sent data packets, including retransmissions",     SrtDataValueType::SRT_LONG_LONG,            prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktSentTotal)) - reinterpret_cast<uintptr_t>(&mold),             0}, \
{"pktRecvTotal",            "total number of received packets",                                 SrtDataValueType::SRT_LONG_LONG,            prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktRecvTotal)) - reinterpret_cast<uintptr_t>(&mold),             0}, \
{"pktSndLossTotal",         "total number of lost packets (sender side)",                       SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktSndLossTotal)) - reinterpret_cast<uintptr_t>(&mold),          0}, \
{"pktRcvLossTotal",         "total number of lost packets (receiver side)",                     SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktRcvLossTotal)) - reinterpret_cast<uintptr_t>(&mold),          0}, \
{"pktRetransTotal",         "total number of retransmitted packets",                            SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktRetransTotal)) - reinterpret_cast<uintptr_t>(&mold),          0}, \
{"pktSentACKTotal",         "total number of sent ACK packets",                                 SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktSentACKTotal)) - reinterpret_cast<uintptr_t>(&mold),          0}, \
{"pktRecvACKTotal",         "total number of received ACK packets",                             SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktRecvACKTotal)) - reinterpret_cast<uintptr_t>(&mold),          0}, \
{"pktSentNAKTotal",         "total number of sent NAK packets",                                 SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktSentNAKTotal)) - reinterpret_cast<uintptr_t>(&mold),          0}, \
{"pktRecvNAKTotal",         "total number of received NAK packets",                             SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktRecvACKTotal)) - reinterpret_cast<uintptr_t>(&mold),          0}, \
{"usSndDurationTotal",      "total time duration when UDT is sending data (no idle time)",      SrtDataValueType::SRT_LONG_LONG,            prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.usSndDurationTotal)) - reinterpret_cast<uintptr_t>(&mold),       0}, \
{"pktSndDropTotal",         "number of too-late-to-send dropped packets",                       SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktSndDropTotal)) - reinterpret_cast<uintptr_t>(&mold),          0}, \
{"pktRcvDropTotal",         "number of too-late-to play missing packets",                       SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktRcvDropTotal)) - reinterpret_cast<uintptr_t>(&mold),          0}, \
{"pktRcvUndecryptTotal",    "number of undecrypted packets",                                    SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktRcvUndecryptTotal)) - reinterpret_cast<uintptr_t>(&mold),     0}, \
{"byteSentTotal",           "total number of sent data bytes, including retransmissions",       SrtDataValueType::SRT_UNSIGNED_LONG_LONG,   prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.byteSentTotal)) - reinterpret_cast<uintptr_t>(&mold),            0}, \
{"byteRecvTotal",           "total number of received bytes",                                   SrtDataValueType::SRT_UNSIGNED_LONG_LONG,   prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.byteRecvTotal)) - reinterpret_cast<uintptr_t>(&mold),            0}, \
{"byteRcvLossTotal",        "total number of lost bytes",                                       SrtDataValueType::SRT_UNSIGNED_LONG_LONG,   prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.byteRcvLossTotal)) - reinterpret_cast<uintptr_t>(&mold),         0}, \
{"byteRetransTotal",        "total number of retransmitted bytes",                              SrtDataValueType::SRT_UNSIGNED_LONG_LONG,   prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.byteRetransTotal)) - reinterpret_cast<uintptr_t>(&mold),         0}, \
{"byteSndDropTotal",        "number of too-late-to-send dropped bytes",                         SrtDataValueType::SRT_UNSIGNED_LONG_LONG,   prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.byteSndDropTotal)) - reinterpret_cast<uintptr_t>(&mold),         0}, \
{"byteRcvDropTotal",        "number of too-late-to play missing bytes (estimation)",            SrtDataValueType::SRT_UNSIGNED_LONG_LONG,   prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.byteRcvDropTotal)) - reinterpret_cast<uintptr_t>(&mold),         0}, \
{"byteRcvUndecryptTotal",   "number of undecrypted bytes",                                      SrtDataValueType::SRT_UNSIGNED_LONG_LONG,   prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.byteRcvUndecryptTotal)) - reinterpret_cast<uintptr_t>(&mold),    0}, \
{"pktSent",                 "number of sent data packets, including retransmissions",           SrtDataValueType::SRT_LONG_LONG,            prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktSent)) - reinterpret_cast<uintptr_t>(&mold),                  0}, \
{"pktRecv",                 "number of received packets",                                       SrtDataValueType::SRT_LONG_LONG,            prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktRecv)) - reinterpret_cast<uintptr_t>(&mold),                  0}, \
{"pktSndLoss",              "number of lost packets (sender side)",                             SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktSndLoss)) - reinterpret_cast<uintptr_t>(&mold),               0}, \
{"pktRcvLoss",              "number of lost packets (receiver side)",                           SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktRcvLoss)) - reinterpret_cast<uintptr_t>(&mold),               0}, \
{"pktRetrans",              "number of retransmitted packets",                                  SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktRetrans)) - reinterpret_cast<uintptr_t>(&mold),               0}, \
{"pktRcvRetrans",           "number of retransmitted packets received",                         SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktRcvRetrans)) - reinterpret_cast<uintptr_t>(&mold),            0}, \
{"pktSentACK",              "number of sent ACK packets",                                       SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktSentACK)) - reinterpret_cast<uintptr_t>(&mold),               0}, \
{"pktRecvACK",              "number of received ACK packets",                                   SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktRecvACK)) - reinterpret_cast<uintptr_t>(&mold),               0}, \
{"pktSentNAK",              "number of sent NAK packets",                                       SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktSentNAK)) - reinterpret_cast<uintptr_t>(&mold),               0}, \
{"pktRecvNAK",              "number of received NAK packets",                                   SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktRecvNAK)) - reinterpret_cast<uintptr_t>(&mold),               0}, \
{"mbpsSendRate",            "sending rate in Mb/s",                                             SrtDataValueType::SRT_DOUBLE,               prometheus::MetricType::Gauge,      {},     reinterpret_cast<uintptr_t>(&(mold.mbpsSendRate)) - reinterpret_cast<uintptr_t>(&mold),             0}, \
{"mbpsRecvRate",            "receiving rate in Mb/s",                                           SrtDataValueType::SRT_DOUBLE,               prometheus::MetricType::Gauge,      {},     reinterpret_cast<uintptr_t>(&(mold.mbpsRecvRate)) - reinterpret_cast<uintptr_t>(&mold),             0}, \
{"usSndDuration",           "busy sending time (i.e., idle time exclusive)",                    SrtDataValueType::SRT_LONG_LONG,            prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.usSndDuration)) - reinterpret_cast<uintptr_t>(&mold),            0}, \
{"pktReorderDistance",      "size of order discrepancy in received sequences",                  SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktReorderDistance)) - reinterpret_cast<uintptr_t>(&mold),       0}, \
{"pktRcvAvgBelatedTime",    "average time of packet delay for belated packets",                 SrtDataValueType::SRT_DOUBLE,               prometheus::MetricType::Gauge,      {},     reinterpret_cast<uintptr_t>(&(mold.pktRcvAvgBelatedTime)) - reinterpret_cast<uintptr_t>(&mold),     0}, \
{"pktRcvBelated",           "number of received AND IGNORED packets for coming too late",       SrtDataValueType::SRT_LONG_LONG,            prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktRcvBelated)) - reinterpret_cast<uintptr_t>(&mold),            0}, \
{"pktSndDrop",              "number of too-late-to-send dropped packets",                       SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktSndDrop)) - reinterpret_cast<uintptr_t>(&mold),               0}, \
{"pktRcvDrop",              "number of too-late-to play missing packets",                       SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktRcvDrop)) - reinterpret_cast<uintptr_t>(&mold),               0}, \
{"pktRcvUndecrypt",         "number of undecrypted packets",                                    SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktRcvUndecrypt)) - reinterpret_cast<uintptr_t>(&mold),          0}, \
{"byteSent",                "number of sent data bytes, including retransmissions",             SrtDataValueType::SRT_UNSIGNED_LONG_LONG,   prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.byteSent)) - reinterpret_cast<uintptr_t>(&mold),                 0}, \
{"byteRecv",                "number of received bytes",                                         SrtDataValueType::SRT_UNSIGNED_LONG_LONG,   prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.byteRecv)) - reinterpret_cast<uintptr_t>(&mold),                 0}, \
{"byteRcvLoss",             "number of retransmitted bytes",                                    SrtDataValueType::SRT_UNSIGNED_LONG_LONG,   prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.byteRcvLoss)) - reinterpret_cast<uintptr_t>(&mold),              0}, \
{"byteRetrans",             "number of retransmitted bytes",                                    SrtDataValueType::SRT_UNSIGNED_LONG_LONG,   prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.byteRetrans)) - reinterpret_cast<uintptr_t>(&mold),              0}, \
{"byteSndDrop",             "number of too-late-to-send dropped bytes",                         SrtDataValueType::SRT_UNSIGNED_LONG_LONG,   prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.byteSndDrop)) - reinterpret_cast<uintptr_t>(&mold),              0}, \
{"byteRcvDrop",             "number of too-late-to play missing bytes (estimation)",            SrtDataValueType::SRT_UNSIGNED_LONG_LONG,   prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.byteRcvDrop)) - reinterpret_cast<uintptr_t>(&mold),              0}, \
{"byteRcvUndecrypt",        "number of undecrypted bytes",                                      SrtDataValueType::SRT_UNSIGNED_LONG_LONG,   prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.byteRcvUndecrypt)) - reinterpret_cast<uintptr_t>(&mold),         0}, \
{"usPktSndPeriod",          "packet sending period, in microseconds",                           SrtDataValueType::SRT_DOUBLE,               prometheus::MetricType::Gauge,      {},     reinterpret_cast<uintptr_t>(&(mold.usPktSndPeriod)) - reinterpret_cast<uintptr_t>(&mold),           0}, \
{"pktFlowWindow",           "flow window size, in number of packets",                           SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktFlowWindow)) - reinterpret_cast<uintptr_t>(&mold),            0}, \
{"pktCongestionWindow",     "congestion window size, in number of packets",                     SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktCongestionWindow)) - reinterpret_cast<uintptr_t>(&mold),      0}, \
{"pktFlightSize",           "number of packets on flight",                                      SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktFlightSize)) - reinterpret_cast<uintptr_t>(&mold),            0}, \
{"msRTT",                   "RTT, in milliseconds",                                             SrtDataValueType::SRT_DOUBLE,               prometheus::MetricType::Gauge,      {},     reinterpret_cast<uintptr_t>(&(mold.msRTT)) - reinterpret_cast<uintptr_t>(&mold),                    0}, \
{"mbpsBandwidth",           "estimated bandwidth, in Mb/s",                                     SrtDataValueType::SRT_DOUBLE,               prometheus::MetricType::Gauge,      {},     reinterpret_cast<uintptr_t>(&(mold.mbpsBandwidth)) - reinterpret_cast<uintptr_t>(&mold),            0}, \
{"byteAvailSndBuf",         "available UDT sender buffer size",                                 SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.byteAvailSndBuf)) - reinterpret_cast<uintptr_t>(&mold),          0}, \
{"byteAvailRcvBuf",         "available UDT receiver buffer size",                               SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.byteAvailRcvBuf)) - reinterpret_cast<uintptr_t>(&mold),          0}, \
{"mbpsMaxBW",               "Transmit Bandwidth ceiling (Mbps)",                                SrtDataValueType::SRT_DOUBLE,               prometheus::MetricType::Gauge,      {},     reinterpret_cast<uintptr_t>(&(mold.mbpsMaxBW)) - reinterpret_cast<uintptr_t>(&mold),                0}, \
{"byteMSS",                 "MTU",                                                              SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.byteMSS)) - reinterpret_cast<uintptr_t>(&mold),                  0}, \
{"pktSndBuf",               "UnACKed packets in UDT sender",                                    SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktSndBuf)) - reinterpret_cast<uintptr_t>(&mold),                0}, \
{"byteSndBuf",              "UnACKed bytes in UDT sender",                                      SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.byteSndBuf)) - reinterpret_cast<uintptr_t>(&mold),               0}, \
{"msSndBuf",                "UnACKed timespan (msec) of UDT sender",                            SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.msSndBuf)) - reinterpret_cast<uintptr_t>(&mold),                 0}, \
{"msSndTsbPdDelay",         "Timestamp-based Packet Delivery Delay",                            SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.msSndTsbPdDelay)) - reinterpret_cast<uintptr_t>(&mold),          0}, \
{"pktRcvBuf",               "Undelivered packets in UDT receiver",                              SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktRcvBuf)) - reinterpret_cast<uintptr_t>(&mold),                0}, \
{"byteRcvBuf",              "Undelivered bytes of UDT receiver",                                SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.byteRcvBuf)) - reinterpret_cast<uintptr_t>(&mold),               0}, \
{"msRcvBuf",                "Undelivered timespan (msec) of UDT receiver",                      SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.msRcvBuf)) - reinterpret_cast<uintptr_t>(&mold),                 0}, \
{"msRcvTsbPdDelay",         "Timestamp-based Packet Delivery Delay",                            SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.msRcvTsbPdDelay)) - reinterpret_cast<uintptr_t>(&mold),          0}, \
{"pktSndFilterExtraTotal",  "number of control packets supplied by packet filter",              SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktSndFilterExtraTotal)) - reinterpret_cast<uintptr_t>(&mold),   0}, \
{"pktRcvFilterExtraTotal",  "number of control packets received and not supplied back",         SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktRcvFilterExtraTotal)) - reinterpret_cast<uintptr_t>(&mold),   0}, \
{"pktRcvFilterSupplyTotal", "number of packets that the filter supplied extra",                 SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktRcvFilterSupplyTotal)) - reinterpret_cast<uintptr_t>(&mold),  0}, \
{"pktRcvFilterLossTotal",   "number of packet loss not coverable by filter",                    SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktRcvFilterLossTotal)) - reinterpret_cast<uintptr_t>(&mold),    0}, \
{"pktSndFilterExtra",       "number of control packets supplied by packet filter",              SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktSndFilterExtra)) - reinterpret_cast<uintptr_t>(&mold),        0}, \
{"pktRcvFilterExtra",       "number of control packets received and not supplied back",         SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktRcvFilterExtra)) - reinterpret_cast<uintptr_t>(&mold),        0}, \
{"pktRcvFilterSupply",      "number of packets that the filter supplied extra",                 SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktRcvFilterSupply)) - reinterpret_cast<uintptr_t>(&mold),       0}, \
{"pktRcvFilterLoss",        "number of packet loss not coverable by filter",                    SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktRcvFilterLoss)) - reinterpret_cast<uintptr_t>(&mold),         0}, \
}

// New items in libsrt v1.5.0
/* 
{"pktReorderTolerance",     "packet reorder tolerance value",                                   SrtDataValueType::SRT_INT,                  prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktReorderTolerance)) - reinterpret_cast<uintptr_t>(&mold),      0}, \
{"pktSentUniqueTotal",      "total number of data packets sent by the application",             SrtDataValueType::SRT_LONG_LONG,            prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktSentUniqueTotal)) - reinterpret_cast<uintptr_t>(&mold),       0}, \
{"pktRecvUniqueTotal",      "total number of packets to be received by the application",        SrtDataValueType::SRT_LONG_LONG,            prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktRecvUniqueTotal)) - reinterpret_cast<uintptr_t>(&mold),       0}, \
{"byteSentUniqueTotal",     "total number of data bytes, sent by the application",              SrtDataValueType::SRT_UNSIGNED_LONG_LONG,   prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.byteSentUniqueTotal)) - reinterpret_cast<uintptr_t>(&mold),      0}, \
{"byteRecvUniqueTotal",     "total number of data bytes to be received by the application",     SrtDataValueType::SRT_UNSIGNED_LONG_LONG,   prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.byteRecvUniqueTotal)) - reinterpret_cast<uintptr_t>(&mold),      0}, \
{"pktSentUnique",           "number of data packets sent by the application",                   SrtDataValueType::SRT_LONG_LONG,            prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktSentUnique)) - reinterpret_cast<uintptr_t>(&mold),            0}, \
{"pktRecvUnique",           "number of packets to be received by the application",              SrtDataValueType::SRT_LONG_LONG,            prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.pktRecvUnique)) - reinterpret_cast<uintptr_t>(&mold),            0}, \
{"byteSentUnique",          "number of data bytes, sent by the application",                    SrtDataValueType::SRT_UNSIGNED_LONG_LONG,   prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.byteSentUnique)) - reinterpret_cast<uintptr_t>(&mold),           0}, \
{"byteRecvUnique",          "number of data bytes to be received by the application",           SrtDataValueType::SRT_UNSIGNED_LONG_LONG,   prometheus::MetricType::Counter,    {},     reinterpret_cast<uintptr_t>(&(mold.byteRecvUnique)) - reinterpret_cast<uintptr_t>(&mold),           0}, \
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
    prometheus::MetricType metricType;  // metric type after convertion
    std::vector<MetricLabel> label;     // label list
                                        // (special labels not registered
                                        // as common labels are stored here)
    size_t offset;                      // offset in "struct CBytePerfMon"
    char data[STAT_DATA_BUFF_LEN];      // a buffer for storing the value
} SrtDataMapper;


class SrtExpCollector : public prometheus::Collectable {
 public:
    explicit SrtExpCollector(std::shared_ptr<SrtExporter> srtExp);

    SrtExpRet LabelRegister(const char *name, const char *value,
                            const char *var);
    SrtExpRet SrtSockRegister(SRTSOCKET *sock, int sockNum);

 protected:
    void ClearTranslated() const {_translated->clear();}
    // translate SRT data to metrics to be sent
    void ValueToMetricFamily(const SrtDataMapper *map, prometheus::MetricFamily *metric,
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
    std::shared_ptr<std::vector<prometheus::MetricFamily>> _translated
        = std::make_shared<std::vector<prometheus::MetricFamily>>();
    // filtered entries according to cofiguration
    std::shared_ptr<std::vector<SrtDataMapper>> _filteredData
        = std::make_shared<std::vector<SrtDataMapper>>();
};

class SrtExpCollectorModeOne : public SrtExpCollector {
// mode 1: collect on request
 public:
    using SrtExpCollector::SrtExpCollector;
    std::vector<prometheus::MetricFamily> Collect() const override;

 private:
    // convert data from SRT library API to local format
    void DataAdaptor(SRT_TRACEBSTATS *stats) const;
    // call `ValueToMetricFamily` for each variable
    void MetricTranslation(SRTSOCKET sock) const;
};

}  // namespace srt_exporter
