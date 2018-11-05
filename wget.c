#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

// Global Variables
const unsigned int MAX_SIZE = 1024;
const unsigned int MAX_PORT = 65535;

// Remove maybe
struct host_info {
    char *path;
    char *file;
};

void fatal(char *message) {
    char error_message[100] = {0};

    strncpy(error_message, "[!!] Fatal Error ", strlen(error_message));
    strncat(error_message, message, 83);
    
    // Print a system error message
    perror(error_message);
    exit(-1);
}

/**
 * This function will determine if there is any port number in the URL.
 * If it finds a port number it will return the position of the first number(":" + 1)
 * Else it will return 0
 */
static int port_position(const char *url) {
    for (unsigned int i = 0; url[i] != '\0' && url[i] != '/'; i++) {
        if (url[i] == ':' && isdigit(url[i+1])) {
            return i+1;
        }
    }
    return 0;
}

/**
 * This function extracts the port number from the URL
 * url - pointer to the URL
 * port - pointer that will hold the port number
 * Returns the cursor on the URL after extraction of the port
 */
static unsigned int get_port(const char *url, char *port) {
    unsigned int url_i;

    // Extract port number from URL
    if (url_i = port_position(url)) {
        for (unsigned int i = 0; url[url_i] != '\0' && url[url_i] != '/'; i++, url_i++) {
            if (i > 4) {
                port[i] = '\0';
                break;
            }
            if (isdigit(url[url_i])) 
                port[i] = url[url_i];
            else 
                fatal("invalid port number");
        }
    } else {
        strcpy(port, "80\0");
    }

    // Is port within valid range?
    if ((atoi(port)) > MAX_PORT) {
        fatal("Port is greater than max number");
    }
    return url_i;
}

/**
 * This function extracts the hostname from the URL
 * url - pointer to the URL
 * hostname - pointer that will hold the hostname
 * Returns the cursor on the URL after extraction of the hostname
 */
static unsigned int get_hostname(const char *url, char *hostname) {
    unsigned int i;
    for (i = 0; url[i] != '\0' && url[i] != '/' && url[i] != ':'; i++) {
        if (i >= MAX_SIZE) fatal("too long hostname in URL");
        hostname[i] = url[i];
    }
    hostname[i] = '\0';
    return i;
}

/**
 * This function will format the URL
 * ATM just removing multiple /'s, but might be improved
 */
void format_url(char *p){
    char *end;
    if (! ((end = strchr(p, ';')) || (end = strchr(p, '?')) ||
            (end = strchr(p, '#'))))
        end = p + strlen(p);
    
    while (p < end) {
        if (*p == '/') {
            if (*(p+1) == '/') {
                // Remove multiple /'s
                while (*(p+1) == '/') {
                    char *orig = p, *dest = p+1;
                    while ((*orig++ = *dest++));
                    end = orig-1;
                }
            } else 
                p++;
        } else
            p++;
    }
}

/**
 * This function will extract the path from the URL
 */
static unsigned int get_path(const char *url, unsigned int cursor, char *path) {
    if (url[cursor] == '/') {
        unsigned int i;
        for (i = 0; url[cursor] != '\0'; i++, cursor++) {
            if (i > MAX_SIZE) fatal("too long path in URL");
            path[i] = url[cursor];
        }
        path[i] = '\0';
        format_url(path);
        return 1;
    }
    return 0;
}

/**
 * This function retrieves the filename from the URL.
 * It will ignore everything after "#, ;, ?"
 */
static void get_filename(const char *path, char *filename) {
    char *start;
    unsigned int i = 0;

    start = strrchr(path, '/') + 1;
    while (start[i] != '\0') {
        if (i >= MAX_SIZE) fatal("filename too long");

        if (!(start[i] == '#' || start[i] == '?' || start[i] == ';')) {
            filename[i] = start[i];
            i++;
        } else
            break;
    }
    filename[i] = '\0';
}

static unsigned int parse_url(char *src_url, struct host_info *h, char *hostname, char *port) {
    char path[MAX_SIZE], filename[MAX_SIZE];
    unsigned int url_i, port_i;

    memset(path, 0, MAX_SIZE);
    memset(filename, 0, MAX_SIZE);
    if (strncmp(src_url, "http://", 7) == 0) {
        url_i = get_hostname(src_url + 7, hostname);
        port_i = get_port(src_url + 7 + url_i, port);
        
        if (get_path(src_url, 7 + url_i + port_i, path)) {
            h->path = path;
            printf("Port: %s\n", port);
            printf("Host: %s\n", hostname);
            printf("Path: %s\n", h->path);

            if (h->file == NULL) {
                get_filename(path, filename);
                h->file = filename;
            }
            printf("Filename: %s\n", h->file);
            return 1;
        }
    } else {
        return 0;
    }
}

/**
 * https://github.com/angrave/SystemProgramming/wiki/Networking,-Part-2:-Using-getaddrinfo
 */
static int conn_host(const char *host, const char *port) {
    struct sockaddr_in target_addr;
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

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) != 1)
            break;
        else perror("connection error");
        close(sockfd);
    }
    freeaddrinfo(res);
    if (!p) fatal("cannot connect");

    printf("%d\n", sockfd);
    return sockfd;
}

static void usage(void) {
    printf("Usage: wget [-f] <host>\n"
        "Options are:\n"
        "   -f: specify the filename to be saved\n");
    exit(-1);
}



int main(int argc, char **argv) {
    char *file = {0}, port[6] = {0};
    struct host_info target;
    char hostname[MAX_SIZE];

    memset(hostname, 0, MAX_SIZE);
    if (argc < 2){
        usage();
    }

    unsigned int c;
    target.file = '\0';
    while ((c = getopt(argc, argv, "f:")) != -1) {
        switch (c) {
            case 'f':
                target.file = optarg;
                break;
            default:
                usage();
        }
    }

    // Check file and URL
    if (argv[optind]) {
        if (!parse_url(argv[optind], &target, hostname, port)) {
            fatal("Invalid URL. Only supports http");
        }
        uint32_t socket = conn_host(hostname, port);
        close(socket);
    } else {
        usage();
    }

    return 0;
}