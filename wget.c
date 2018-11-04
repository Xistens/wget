#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

// Global Variables
const unsigned int MAX_SIZE = 1024;
const unsigned int MAX_PORT = 65535;

struct host_info {
    char *host;
    uint32_t port;
    char *path;
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
static unsigned int get_port(const char *url, uint32_t *port) {
    char tmp_port[6] = {0};
    unsigned int url_i;

    // Extract port number from URL
    if (url_i = port_position(url)) {
        for (unsigned int i = 0; url[url_i] != '\0' && url[url_i] != '/'; i++, url_i++) {
            if (i > 4) {
                tmp_port[i] = '\0';
                break;
            }
            if (isdigit(url[url_i])) tmp_port[i] = url[url_i];
            else fatal("invalid port number");
        }
    } else {
        strcpy(tmp_port, "80\0");
    }

    // Is port within valid range?
    if ((*port = atoi(tmp_port)) > MAX_PORT) {
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

static void parse_url(char *src_url, struct host_info *h) {
    char hostname[MAX_SIZE], path[MAX_SIZE], filename[MAX_SIZE];
    unsigned int url_i, port_i;
    uint32_t port;

    memset(hostname, 0, MAX_SIZE);
    memset(path, 0, MAX_SIZE);
    memset(filename, 0, MAX_SIZE);
    if (strncmp(src_url, "http://", 7) == 0) {
        url_i = get_hostname(src_url + 7, hostname);
        port_i = get_port(src_url + 7 + url_i, &port);
        
        if (get_path(src_url, 7 + url_i + port_i, path)) {
            h->port = port;
            h->host = hostname;
            h->path = path;
            printf("Port: %d\n", h->port);
            printf("Host: %s\n", h->host);
            printf("Path: %s\n", h->path);
            get_filename(path, filename);
            printf("Filename: %s\n", filename);
        }
    } else {
        fatal("not an HTTP URL");
    }
}

static void usage(void) {
    printf("Usage: wget [-f] <host>\n"
        "Options are:\n"
        "   -f: specify the filename to be saved\n");
    exit(-1);
}



int main(int argc, char **argv) {
    char *file = {0};
    struct host_info target;

    if (argc < 2){
        usage();
    }

    int c;
    while ((c = getopt(argc, argv, "f:")) != -1) {
        switch (c) {
            case 'f':
                file = optarg;
                break;
            default:
                usage();
        }
    }

    // Check file and URL
    if (file && argv[optind]) {
        parse_url(argv[optind], &target);
    } else {
        usage();
    }

    return 0;
}