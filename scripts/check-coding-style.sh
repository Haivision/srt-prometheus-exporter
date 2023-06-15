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

########
# Main #
########

pip install cpplint

output1=$(cpplint --recursive --extensions=hpp,cpp,h,c --exclude=include/srtexp_collector.hpp --quiet ./)

# definition of SRT_DATA_MAP_INITIALIZER in srtexp_collector.hpp needs a long linelength, so handle it separately.
output2=$(cpplint --linelength=320 --quiet ./include/srtexp_collector.hpp)

if [[ -z "$output1" && -z "$output2" ]]; then
	exit 0
else
	exit 1
fi
