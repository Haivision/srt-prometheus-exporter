// head file for srt_exporter's logger

#pragma once

#include <sys/syslog.h>
#include <iostream>
#include <string>

#include "srtexp_define.hpp"


namespace srt_exporter {

// class logger
class logger {
public:
    static void SrtLog_SetDestination(SrtExpLogDestination dest);
    static void SrtLog_SetSyslogLevel(int level);

    static void SrtLog_SyslogError(const char *msg);
    static void SrtLog_Error(const std::string msg);
    static void SrtLog_SyslogNotice(const char *msg);
    static void SrtLog_Notice(const std::string msg);
    static void SrtLog_SyslogDebug(const char *msg);
    static void SrtLog_Debug(const std::string msg);
};

} //namespace srt_exporter
