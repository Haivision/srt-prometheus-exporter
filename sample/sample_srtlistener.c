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
    char exporterName[32] = "sample_srt_listener";
    int id = -1;

    SRTSOCKET srtsock = SRT_INVALID_SOCK;
    SRTSOCKET *p = &srtsock;
    SRTSOCKET in = SRT_INVALID_SOCK;
    SRTSOCKET *in_p = &in;
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    int sa_len = sizeof(addr);
    char recv[20] = {0};
    int continueFlag = 0;

    srt_startup();
    srtexp_c_init((const char *)configFile);

    srtsock = srt_create_socket();
    srt_bind(srtsock, (const struct sockaddr *)&addr, sa_len);
    srt_listen(srtsock, 1);

    if (SRT_EXP_C_SUCCESS != srtexp_c_start((const char *)exporterName, &id)) {
        srt_close(srtsock);
        srt_cleanup();

        return -1;
    }
    srtexp_c_srt_socket_register(p, 1, id);

    do {
        in = srt_accept(srtsock, (struct sockaddr *)&addr, &sa_len);
        if (in != SRT_INVALID_SOCK) {
            printf("connection established\n");
            srtexp_c_srt_socket_register(in_p, 1, id);
            break;
        }
        sleep(1);
    } while (1);

    do {
        srt_recv(in, recv, 2048);
        printf("%s\n", recv);
        recv[0] = '\0';
        sleep(1);
        continueFlag += 1;
    } while (continueFlag < 180);

    srtexp_c_stop(NULL, id);
    srtexp_c_deinit();

    srt_close(srtsock);
    srt_cleanup();

    return 0;
}
