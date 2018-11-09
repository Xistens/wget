#ifndef CONNECT_H
#define CONNECT_H
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>


// Function declarations
int send_string(uint32_t sockfd, char *buffer);
int conn_host(const char *host, const char *port);

#endif