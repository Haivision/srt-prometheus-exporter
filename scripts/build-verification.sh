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

function exit_trap() {
    if [ $? != 0 ]; then
        echo "Command [$BASH_COMMAND] is failed"
        exit 1
    fi
}

function install_prerequisites () {
    trap exit_trap ERR
    echo "[${FUNCNAME[0]}]: update and install dependent packages."
    apt update && apt upgrade -y
    apt install -y g++ curl cmake pkg-config libcurl4-openssl-dev build-essential zlib1g-dev git libyaml-cpp-dev
    if [[ $(lsb_release -rs) == "20.04" ]]; then
        apt install -y libsrt-dev libsrt-dev
    elif [[ $(lsb_release -rs) == "22.04" ]]; then
        apt install -y libsrt-openssl-dev libsrt-doc
    else
        echo "CI is running on unsupported platform, trying to install SRT libs with wildcard."
        apt install -y libsrt-*
    fi
    cd $there
}

function install_prometheus_cpp () {
    trap exit_trap ERR
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
    trap exit_trap ERR
    echo "[${FUNCNAME[0]}]: build and install SRT Prometheus Exporter."
    make
    cd $there
}

function uninstall_srt_exporter () {
    trap exit_trap ERR
    echo "[${FUNCNAME[0]}]: uninstall SRT Prometheus Exporter."
    make clean
    cd $there
}

function build_sample_apps () {
    trap exit_trap ERR
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

# set the trap on error
trap exit_trap ERR

# call install functions in sequence
install_prerequisites
install_prometheus_cpp
install_srt_exporter
build_sample_apps
uninstall_srt_exporter

exit 0
