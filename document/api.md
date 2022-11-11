# SRT Exporter Library APIs

## SrtExpRet srtexp_init(const char *configFile)

- This API should be called when SRT Exporter library user application starts.

- It loads configuration of the library from a specified path, or from the default location when the input is a NULL pointer.

## SrtExpRet srtexp_deinit()

- Clear all SRT Exporter objects and release memory of loaded configuration.

## SrtExpRet srtexp_start(const char *exporterName, int *id)

- This API starts an SRT Exporter object with or without a specified name.  
It also provides a numeric `id` for the user to identify this object later.  

- `exporterName` helps the library to find the specified configuration for this new SRT Exporter object.  
It is OK to put a NULL pointer for `exporterName` to start an SRT Exporter object using global configuration.  

- You can have multiple SRT Exporter object in one process.  
Each of them has a separate http server on a different port.  

## SrtExpRet srtexp_stop(int id)

- Use this API to stop an SRT Exporter object with the `id` provided by `srtexp_start`.

## SrtExpRet srtexp_stop(const char *exporterName)

- Use this API to stop an SRT Exporter object with the specified name.

## SrtExpRet srtexp_label_register(const char *name, const char *value, int id)

- This API provide an interface for use to add, edit, or remove a label of the SRT Exporter object dynamically.

## SrtExpRet srtexp_srt_socket_register(SRTSOCKET *sock, int sockNum, int id)

- This API register the SRT sockets to track to one SRT Exporter object with the `id` provided by `srtexp_start`.

- You can overwrite the SRT sockets registered before by call this API again.

## SrtExpRet srtexp_set_log_dest(SrtExpLogDestination dest)

- Use this API to change the logging destination of SRT Exporter library.

- `SRT_EXP_STDERR = 0` for standard error, `SRT_EXP_SYSLOG = 1` for syslog.

- Syslog is the default logging destination.

## SrtExpRet srtexp_set_syslog_level(int level)

- Use this API to change the logging level of SRT Exporter library.

- Log level `LOG_ERR`, `LOG_NOTICE`, and `LOG_DEBUG` defined by syslog are expected input.


