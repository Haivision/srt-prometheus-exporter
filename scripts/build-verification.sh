#!/bin/bash

#####################################################################
# SRT - Secure, Reliable, Transport
# Copyright (c) 2023 Sony Group Corporation.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
#####################################################################

# To avoid updating and modifying the files under `.github/workflows`,
# this scripts should be adjusted building process accordingly.
# And `.github/workflows` just calls this script in the workflow pipeline.
# This allows us to maintain the workflow process easier for contributers.

########################
# Function Definitions #
########################

function mark {
    export $1=`pwd`;
}

function install_prerequisites () {
    echo "[${FUNCNAME[0]}]: update and install dependent packages."
    apt update && apt upgrade -y
    apt install -y g++ curl cmake pkg-config libcurl4-openssl-dev build-essential zlib1g-dev git libsrt-dev libyaml-cpp-dev libsrt-dev
    cd $there
}

function install_prometheus_cpp () {
    echo "[${FUNCNAME[0]}]: build and install prometheus-cpp."
    git clone https://github.com/jupp0r/prometheus-cpp
    cd prometheus-cpp
    git submodule init
    git submodule update
    mkdir _build
    cd _build
    cmake .. -DBUILD_SHARED_LIBS=ON
    make -j 4
    make install
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
    cd $there
}

function install_srt_exporter () {
    echo "[${FUNCNAME[0]}]: build and install SRT Prometheus Exporter."
    make
    cd $there
}

function uninstall_srt_exporter () {
    echo "[${FUNCNAME[0]}]: uninstall SRT Prometheus Exporter."
    make clean
    cd $there
}

function build_sample_apps () {
    echo "[${FUNCNAME[0]}]: build SRT Prometheus Exporter sample application."
    cd sample
    make
    cd $there
}

########
# Main #
########

export DEBIAN_FRONTEND=noninteractive

# mark the working space root directory, so that we can come back anytime with `cd $there`
mark there

# call install functions in sequence
install_prerequisites
install_prometheus_cpp
install_srt_exporter
build_sample_apps
uninstall_srt_exporter

exit 0
