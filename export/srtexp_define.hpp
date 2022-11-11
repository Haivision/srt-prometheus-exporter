// head file for srt_exporter common definitions

#pragma once

#define DEFAULT_CONFIG_FILE "/etc/srtexp/srt_exporter.yaml"

// logging destination
enum class SrtExpLogDestination {
    SRT_EXP_STDERR = 0,
    SRT_EXP_SYSLOG = 1,
};

// return value definition
enum class SrtExpRet {
    SRT_EXP_SUCCESS = 0,
    SRT_EXP_FAILURE = -1,
    SRT_EXP_DISABLED = -2,
    SRT_EXP_INVALID_INPUT = -3,
    SRT_EXP_OBJECT_NOT_FOUND = -4,
    SRT_EXP_MALLOC_FAILURE = -5,
    SRT_EXP_CONNECTION_FAILURE = -6,
};

