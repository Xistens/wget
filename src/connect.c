#include "wget.h"
#include "connect.h"
#include "utils.h"

#define EOL "\r\n\r\n"
#define EOL_SIZE 4

//Don't let this application use up all available memory
#define READ_MAX 4096

/**
 * This function accepts a socket FD and a ptr to a destination buffer, it will try
 * to recv the HTTP header (until "\r\n\r\n").
 * It will receive from the socket until max size or null terminator is seen.
 * Returns the size of the read line
 */
int fd_recv_head(const int sockfd, char *dest_buffer, const unsigned int size) {
    char *ptr;
    size_t curr_size = 0;
    int eol_matched = 0;
    
    ptr = dest_buffer;
    while ((curr_size = recv(sockfd, ptr, 1, 0)) == 1) {
        
        // Reached max size?
        if (curr_size == (size - 1)) {
            *(ptr+1) = '\0';
            return strlen(dest_buffer);
        }

        // End-of-line matched?
        if (*ptr == EOL[eol_matched]) {
            eol_matched++;
            if (eol_matched == EOL_SIZE) {
                *(ptr+1-EOL_SIZE) = '\0';
                return strlen(dest_buffer);
            }
        } else {
            eol_matched = 0;
        }

        // Increment the pointer to the next byte
        ptr++;
    }

    return curr_size;
}

/**
 * This function will recv data in chunks then
 * write this data directly to the file
 */
int fd_recv_body(int fd, char *filename) {
    char buffer[5000];
    int len;
    FILE *fp;

    if(!(fp = fopen(filename, "w")))
        fatal("fopen error");

    while ((len = recv(fd, buffer, READ_MAX, 0)) > 0) {
        if (fwrite(buffer, 1, len, fp) != len)
            fatal("fwrite error");
    }

    if(fclose(fp) == EOF)
        fatal("fclose error");
    return len;
}

/**
 * This function accepts a socket FD and a ptr to the null terminated string to send
 * The function will make sure all bytes of the string are sent.
 * Returns 1 on success and 0 on failure.
 */
int send_string(const int sockfd, char *buffer) {
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
    int sockfd, errno;

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