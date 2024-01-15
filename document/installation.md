# Installation

## Requirements / Dependency

### Supported System

- [Ubuntu 20.04 Focal Fossa](https://releases.ubuntu.com/focal/)
- [Ubuntu 22.04 Jammy Jellyfish](https://releases.ubuntu.com/jammy/)

### Prerequisites

The following packages are required to install as common.

```bash
apt update
apt upgrade -y
apt install -y g++ curl cmake pkg-config libcurl4-openssl-dev build-essential zlib1g-dev git libyaml-cpp-dev
```

- Ubuntu 20.04

```bash
apt install -y libsrt-dev libsrt-doc
```

- Ubuntu 22.04

```bash
apt install -y libsrt-openssl-dev libsrt-doc
```

### Dependent Packages

- [prometheus-cpp](https://github.com/jupp0r/prometheus-cpp) library

  > [!NOTE]
  > `prometheus-cpp` debian package is supported on ubuntu 23 or later. see more details for https://github.com/Haivision/srt-prometheus-exporter/issues/11.

  ```bash
  git clone https://github.com/jupp0r/prometheus-cpp
  cd prometheus-cpp
  git submodule init
  git submodule update
  mkdir _build
  cd _build
  cmake .. -DBUILD_SHARED_LIBS=ON
  make -j 4
  make install
  ```

  Check if the following libraries are installed as expectedly.

  ```bash
  ls /usr/local/lib/libprometheus-cpp*
  /usr/local/lib/libprometheus-cpp-core.so        /usr/local/lib/libprometheus-cpp-pull.so        /usr/local/lib/libprometheus-cpp-push.so
  /usr/local/lib/libprometheus-cpp-core.so.1.1    /usr/local/lib/libprometheus-cpp-pull.so.1.1    /usr/local/lib/libprometheus-cpp-push.so.1.1
  /usr/local/lib/libprometheus-cpp-core.so.1.1.0  /usr/local/lib/libprometheus-cpp-pull.so.1.1.0  /usr/local/lib/libprometheus-cpp-push.so.1.1.0
  ```

  Set `/usr/local/lib` to `LD_LIBRARY_PATH` environmental variable to avoid dynamic link error.

  ```bash
  export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
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
