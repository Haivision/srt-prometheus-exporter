# Proposals

## Collection Modes

Currently every time exporter receives http request, it will issue SRT statistics API to collect the statistics based on the socket.
This mode is default, and it is reasonably okay since it is request-based statistics collection without wasting any information or computation.
But the downside could be latency to response against that request with statistics, since it might take time to get all statistics after the request.
Besides, with this request base approach, it is responsible for Prometheus to send the request with enough frequency, not SRT Prometheus Exporter. (e.g. if statistics needs to be collected every seconds, Prometheus is responsible to call the SRT Prometheus Exporter with that frequency to cover the requirement, unless it cannot get the enough statistics.)

Against these situations, the following modes can be proposal to be more flexible.

- **Periodically Collection Mode**

  This mode is set into SRT Prometheus Exporter.
  Once this period is set by configuration, statistics data collector in SRT Prometheus Exporter keeps collecting the statistics with this period to cache those data in the memory.
  And when the request comes in, all cached statistics will be sent to Prometheus.

  This can reduce the burden for Prometheus described above.
  Besides, it can reduce the communication between Prometheus and SRT Prometheus Exporter significantly.

  Downside could be caching schema, since it requires to allocate memory dynamically, it would not be appropriate mechanism to consider real-time like system operation on edge IoT device.

- **Passive Mode**

  User application is responsible to call the API to store the statistics to store into SRT Prometheus Exporter.
  Those statistics will be store in SRT Prometheus Exporter, and send to Prometheus once requested.User can store user-defined statistics as well.
