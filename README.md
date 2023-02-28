# SRT Prometheus Exporter

## Overview

SRT Prometheus Exporter is an implementation for Prometheus exporter for Secure Reliable Transport (SRT) protocol.

<img src="./document/images/overview1.png" alt="overview" width="800">

SRT Prometheus Exporter is a library built on top of [SRT Stats APIs](https://github.com/Haivision/srt/blob/master/docs/API/statistics.md) and [Prometheus Instrumenting Client Library](https://prometheus.io/docs/instrumenting/clientlibs/).
It allows user application to get the statistics for each SRT socket and send those data to Prometheus as metrics on request.
SRT Prometheus Exporter integrates HTTP server in it so that statistics can be scraped by the request from Prometheus.

## Supported Versions

It could be working the following versions or latter, but not recommended to be used except following specific versions since it does not verify with other versions.

- Ubuntu: 20.04 Focal Fossa
- [SRT v1.4.0](https://github.com/Haivision/srt/releases/tag/v1.4.0)

## Tutorials

- [Getting Started](./document/tutorial.md)

## API Document

- [API Document](./document/api.md)

## Development

- [Development Document](./document/design.md)

## License

Mozilla Public License Version 2.0, [LICENSE](./LICENSE)

## Samples

<img src="./document/images/overview2.png" alt="sample_overview" width="800">
