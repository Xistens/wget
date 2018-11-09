#ifndef HTTP_HEADER
#define HTTP_HEADER
typedef struct request_header_t {
    char *name;
    char *value;
} request_header;

typedef struct node_t {
    request_header *header;
    struct node_t *next;
} node;

struct request {
    char *method;
    char *path;
    node *head;
    int hcount, capacity;
};

struct request *new_request(void);
void request_set(struct request *req, char *method, char *path);
node *create_node(request_header *hdr);
void set_header(struct request *req, request_header *hdr);
void request_free(struct request *req);
void send_request(struct request *req);
#endif