# Installation

## Requirements / Dependency

### System

It could be working the following versions or latter, but not recommended to be used except following specific versions since it does not verify with other versions.

- Ubuntu: 20.04 Focal Fossa
- [SRT v1.4.0](https://github.com/Haivision/srt/releases/tag/v1.4.0)

### Prerequisites

The following packages are required to install.

```bash
apt update
apt upgrade -y
apt install -y g++ curl cmake pkg-config libcurl4-openssl-dev build-essential zlib1g-dev git libsrt-dev libyaml-cpp-dev
```

### Dependent Packages

- [prometheus-cpp](https://github.com/jupp0r/prometheus-cpp) library

  ```bash
  git clone https://github.com/jupp0r/prometheus-cpp
  cd prometheus-cpp
  git submodule init
  git submodule update
  mkdir _build
  cd _build
  cmake .. -DBUILD_SHARED_LIBS=ON
  make -j 4
  sudo make install
  ```

  Check if the following libraries are installed as expectedly.

  ```bash
  ls /usr/local/lib/libprometheus-cpp*
  /usr/local/lib/libprometheus-cpp-core.so        /usr/local/lib/libprometheus-cpp-pull.so        /usr/local/lib/libprometheus-cpp-push.so
  /usr/local/lib/libprometheus-cpp-core.so.1.1    /usr/local/lib/libprometheus-cpp-pull.so.1.1    /usr/local/lib/libprometheus-cpp-push.so.1.1
  /usr/local/lib/libprometheus-cpp-core.so.1.1.0  /usr/local/lib/libprometheus-cpp-pull.so.1.1.0  /usr/local/lib/libprometheus-cpp-push.so.1.1.0
  ```

- SRT library (If necessary)

  The stable version can be installed with `apt install libsrt-dev` on `Ubuntu 20.04` version is `v1.4.0`.
  **If SRT library is compiled from code, be sure that the code is reset to tag `v1.4.0`. Otherwise, srt_exporter library compiled would not work with installed SRT library on other devices. Make sure that SRT Exporter library is built and running based on the same version of SRT library.**  

### Build and Install

- Install

  ```bash
  cd srt-prometheus-exporter
  sudo make
  ```

  `libsrtexp.so` will be compiled and installed to `/usr/local/lib`, and related header files will be installed in `/usr/local/include/srtexp`.

- Uninstall

  ```bash
  cd srt-prometheus-exporter
  sudo make clean
  ```
