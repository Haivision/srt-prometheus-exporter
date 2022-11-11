#include "srtexp_logger.hpp"

namespace srt_exporter {

static SrtExpLogDestination srtExpLogDestination = SrtExpLogDestination::SRT_EXP_SYSLOG;
static int syslogLevel = LOG_NOTICE;

void logger::SrtLog_SetDestination(SrtExpLogDestination dest) {
    srtExpLogDestination = dest;
}

void logger::SrtLog_SetSyslogLevel(int level) {
    syslogLevel = level;
}

void logger::SrtLog_SyslogError(const char *msg) {
    if (syslogLevel >= LOG_ERR) {
        syslog(LOG_ERR, "[ERROR] %s\n", msg);
    }
}

void logger::SrtLog_Error(const std::string msg) {
    switch (srtExpLogDestination) {
        case SrtExpLogDestination::SRT_EXP_SYSLOG:
            SrtLog_SyslogError(msg.c_str());
            break;
        case SrtExpLogDestination::SRT_EXP_STDERR:
        default:
            if (syslogLevel >= LOG_ERR) {
                std::cout << msg << std::endl;
            }
    }
}

void logger::SrtLog_SyslogNotice(const char *msg) {
    if (syslogLevel >= LOG_NOTICE) {
        syslog(LOG_NOTICE, "[NOTICE] %s\n", msg);
    }
}

void logger::SrtLog_Notice(const std::string msg) {
    switch (srtExpLogDestination) {
        case SrtExpLogDestination::SRT_EXP_SYSLOG:
            SrtLog_SyslogNotice(msg.c_str());
            break;
        case SrtExpLogDestination::SRT_EXP_STDERR:
        default:
            if (syslogLevel >= LOG_NOTICE) {
                std::cout << msg << std::endl;
            }
    }
}

void logger::SrtLog_SyslogDebug(const char *msg) {
    if (syslogLevel >= LOG_DEBUG) {
        syslog(LOG_DEBUG, "[DEBUG] %s\n", msg);
    }
}

void logger::SrtLog_Debug(const std::string msg)  {
    switch (srtExpLogDestination) {
        case SrtExpLogDestination::SRT_EXP_SYSLOG:
            SrtLog_SyslogDebug(msg.c_str());
            break;
        case SrtExpLogDestination::SRT_EXP_STDERR:
        default:
            if (syslogLevel >= LOG_DEBUG) {
                std::cout << msg << std::endl;
            }
    }
}

};
