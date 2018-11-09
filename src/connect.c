#include "wget.h"
#include "connect.h"
#include "utils.h"

/**
 * This function accepts a socket FD and a ptr to the null terminated string to send
 * The function will make sure all bytes of the string are sent.
 * Returns 1 on success and 0 on failure.
 */
int send_string(uint32_t sockfd, char *buffer) {
    size_t sent_bytes, bytes_to_send;
    bytes_to_send = strlen(buffer);

    while (bytes_to_send > 0) {
        sent_bytes = send(sockfd, buffer, bytes_to_send, 0);
        if (sent_bytes == -1)
            return 0;
        bytes_to_send -= sent_bytes;
        buffer += sent_bytes;

    }
    return -1;
}

/**
 * https://github.com/angrave/SystemProgramming/wiki/Networking,-Part-2:-Using-getaddrinfo
 * https://beej.us/guide/bgnet/html/multi/clientserver.html
 */
int conn_host(const char *host, const char *port) {
    //struct sockaddr_in target_addr;
    struct addrinfo hints, *res, *p;
    uint32_t sockfd, errno;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;            // Use IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;        // TCP
    hints.ai_flags = AI_PASSIVE;            // Fill in IP automatically
    hints.ai_protocol = 0;

    if ((errno = getaddrinfo(host, port, &hints, &res))) {
        fprintf(stderr, "in getaddrinfo: %s\n", gai_strerror(errno));
        exit(1);
    }

    for (p = res; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("socket error");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) != -1)
            break;
        else perror("connection error");
        close(sockfd);
    }
    freeaddrinfo(res);
    if (!p) return -1;

    return sockfd;
}