#include "srtexp_api.hpp"
#include "srtexp_api_c_wrapper.h"

#include "srtexp_define.hpp"


#ifdef __cplusplus
extern "C" {
#endif

SrtExpCRet srtexp_c_init(const char *configFile) {
    return (SrtExpCRet)srtexp_init(configFile);
}

SrtExpCRet srtexp_c_deinit() {
    return (SrtExpCRet)srtexp_deinit();
}

SrtExpCRet srtexp_c_start(const char *exporterName, int *id) { 
    return (SrtExpCRet)srtexp_start(exporterName, id);
}

SrtExpCRet srtexp_c_stop(const char *exporterName, int id) {
    if (exporterName) {
        return (SrtExpCRet)srtexp_stop(exporterName);
    }
    else {
        return (SrtExpCRet)srtexp_stop(id);
    }
}

SrtExpCRet srtexp_c_label_register(const char *name, const char *value, const char *var, int id) {
    return (SrtExpCRet)srtexp_label_register(name, value, var, id);
}

SrtExpCRet srtexp_c_srt_socket_register(SRTSOCKET *sock, int sockNum, int id) {
    return (SrtExpCRet)srtexp_srt_socket_register(sock, sockNum, id);
}

SrtExpCRet srtexp_c_set_log_dest(SrtExpCLogDestination dest) {
    return (SrtExpCRet)srtexp_set_log_dest((SrtExpLogDestination)dest);
}

SrtExpCRet srtexp_c_set_syslog_level(int level) {
    return (SrtExpCRet)srtexp_set_syslog_level(level);
}

#ifdef __cplusplus
};
#endif
