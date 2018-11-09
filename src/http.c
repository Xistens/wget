#include "wget.h"
#include "http.h"
#include "utils.h"
#include "connect.h"

/**
 * Creates a new, empty request.
 */
struct request *new_request(void) {
    struct request *req = c_malloc(sizeof(struct request));

    req->method = "GET";
    req->head = NULL;
    return req;
}

/**
 * Sets the request's method and path
 */
void request_set(struct request *req, char *method, char *path) {
    req->method = method;
    req->path = path;
}

/**
 * This function accepts a ptr to request_header and creates a new node
 */
node *create_node(request_header *hdr) {
    node *new_node = c_malloc(sizeof(node));

    new_node->header = hdr;
    new_node->next = NULL;

    return new_node;
}

/**
 * Set the request named NAME to VALUE.
 */
void set_header(struct request *req, request_header *hdr) {
    node *current = NULL, *prev;

    if (req->head == NULL) {
        req->head = create_node(hdr);
    }
    else {
        current = prev = req->head;

        // If it exist already, update it
        while (current != NULL) {
            if(current->header != NULL) {
                // Not case-sensitive
                if (0 == strcasecmp(hdr->name, current->header->name)) {
                    current->header->name = hdr->name;
                    current->header->value = hdr->value;
                    return;
                }
            }
            current = current->next;
        }
        
        current = req->head;
        // Loop through list
        while (current->next != NULL) {
            prev = current;
            current = current->next;
        }
        current->next = create_node(hdr);
    }
}

/**
 * Release all headers and request
 */
void request_free(struct request *req) {
    node *current = req->head;

    while (req->head != NULL) {
        current = req->head;
        req->head = current->next;

        if (current->header != NULL)
            free(current->header);
        free(current);
    }
    free(req);
}

/**
 * Helper function for appending text to string with memcpy
 */
static uint32_t append(char *p, char *str) {
    int len = strlen(str);
    memcpy(p, str, len);
    return len;
}

/**
 * This function will build the HTTP request header.
 * TODO: make it accept a socket and send the request
 */
void send_request(struct request *req, int fd){
    node *current = req->head;
    char *request_string, *p;
    uint32_t size = 0, errno;

    /*METHOD " " ARG " " "HTTP/1.0" "\r\n" */
    size += strlen(req->method) + 1 + strlen(req->path) + 1 + 8 + 2;

    while (current != NULL) {
        /* NAME ": " "VALUE "\r\n */
        size += strlen(current->header->name) + 2 + strlen(current->header->value) + 2;
        current = current->next;
    }

    /* "\r\n\0" */
    size += 3;
    p = request_string = c_malloc(size * sizeof(char));

    p += append(p, req->method); *p++ = ' ';
    p += append(p, req->path); *p++ = ' ';
    memcpy(p, "HTTP/1.1\r\n", 10); p += 10;

    current = req->head;
    while (current != NULL) {
        p += append(p, current->header->name);
        *p++ = ':', *p++ = ' ';
        p += append(p, current->header->value);
        *p++ = '\r', *p++ = '\n';

        current = current->next;
    }
    *p++ = '\r', *p++ = '\n', *p++ = '\0';

    // Check if size is correct
    assert(p - request_string == size);

    #ifdef DEBUG
        printf("\nHTTP Request Header:\n%s\n", request_string);
    #endif

    free(p);
    free(request_string);
    //errno = write(fd, request_string, size -1);
}
