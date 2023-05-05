/*
 * SRT - Secure, Reliable, Transport
 * Copyright (c) 2022 Sony Group Corporation.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <srt/srt.h>

#include "srtexp/srtexp_api_c_wrapper.h"


int main() {
    char configFile[32] = "../config/srt_exporter.yaml";
    char exporterName[32] = "sample_srt_caller";
    int id = -1;

    SRTSOCKET srtsock = SRT_INVALID_SOCK;
    SRTSOCKET *p = &srtsock;
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    int sa_len = sizeof(addr);
    int continueFlag = 1;

    srt_startup();
    srtexp_c_init((const char *)configFile);

    srtsock = srt_create_socket();

    if (SRT_EXP_C_SUCCESS != srtexp_c_start((const char *)exporterName, &id)) {
        srt_close(srtsock);
        srt_cleanup();

        return -1;
    }
    srtexp_c_label_register("application", "sample_srtcaller", NULL, id);
    srtexp_c_label_register("importance", "special", "pktSentTotal", id);
    srtexp_c_label_register("importance", "normal", "pktSentTotal", id);
    //srtexp_c_label_register("importance", NULL, "pktSentTotal", id);
    srtexp_c_srt_socket_register(p, 1, id);

    do {
        printf("connecting\n");
        if (SRT_INVALID_SOCK != srt_connect(srtsock, (const struct sockaddr *)&addr, sa_len)) {
            continueFlag = 0;
        }
        sleep(1);
    } while (continueFlag);

    continueFlag = 0;
    do {
        srt_send(srtsock, "OLA~OLA~OLA~OLA~", 17);
        sleep(1);
        continueFlag += 1;
    } while (continueFlag < 120);

    srtexp_c_stop(NULL, id);
    srtexp_c_deinit();

    srt_close(srtsock);
    srt_cleanup();

    return 0;
}
