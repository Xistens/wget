#ifndef CONNECT_H
#define CONNECT_H
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>


// Function declarations
int send_string(const int sockfd, char *buffer);
int conn_host(const char *host, const char *port);
int fd_recv_head(const int sockfd, char *dest_buffer, const unsigned int size);
int fd_recv_body(int fd, char *filename);

#endif