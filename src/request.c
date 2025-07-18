#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "request.h"
#include "server.h"
#include "socket_comm.h"
#include "utils.h"
#include "response.h"
#include "mime.h"

enum Http_method_t {
    INVALID, GET, HEAD, POST, PUT, DELETE
};

struct Http_method_map_entry {
    const char *name;
    enum Http_method_t type;
} const http_method_map[] = {
    { "GET", GET },
    { "HEAD", HEAD },
    { "POST", POST },
    { "PUT", PUT },
    { "DELETE", DELETE },
    { 0, INVALID }
};

struct Http_request_t {
    char req_line[REQ_LINE_SIZE_MAX];
    char method[METHOD_SIZE_MAX];
    char uri[URI_SIZE_MAX];
    char version[VERSION_SIZE_MAX];
    char *req_fields[REQ_FIELDS_MAX];
    char msg_body[MSG_BODY_SIZE_MAX];
};

static int open_file(const char *path, const char *modes, FILE **fp)
{
    if ((*fp = fopen(path, modes)) == NULL) {
        switch (errno)
        {
        case ENOENT: // No such file or directory
            return 404;
        case EACCES: // Permission denied
            return 403;
        case ENOTDIR: // Not a directory
            return 404;
        default:
            handle_error("fopen");
        }   
    }

    return 200;
}

static int check_file(char *uri, struct Http_response_t *http_resp)
{   
    char path[sizeof(SERVER_ROOT) + URI_SIZE_MAX] = { 0 };
    
    convert_uri_to_path(uri, path);

    if (!is_within_root(path) || !is_file(path))
        return 403;

    const mime_t mime_type = get_mime_type(path);
    add_response_header(http_resp, "Content-Type", mime_type.type);
        
    FILE *fp;
    int status_code = 200;

    if ((status_code = open_file(path, mime_type.read_mode, &fp)) != 200)
        return status_code;
    fclose(fp);
    
    char *file_size_str = get_file_size_str(path);
    add_response_header(http_resp, "Content-Length", file_size_str);

    strcpy(http_resp->filepath, path);
    http_resp->filesize = get_file_size(path);

    return status_code;
}

// map method name (input string) to enum representation using lookup table
static enum Http_method_t parse_http_method(const char *method)
{   
    const struct Http_method_map_entry *entry;
    for (entry = http_method_map; entry->name; entry++)
        if (strncmp(entry->name, method, strlen(entry->name)) == 0)
            break;
        
    return entry->type;
}

// retrieve http request line elements
static void parse_http_req_line(struct Http_request_t *http_req)
{
    char req_line[REQ_LINE_SIZE_MAX + 1] = { 0 };
    char *method, *uri, *version;

    strncpy(req_line, http_req->req_line, REQ_LINE_SIZE_MAX);

    method = strtok(req_line, " ");
    uri = strtok(NULL, " ");
    version = strtok(NULL, "\r\n");

    strncpy(http_req->method, method, METHOD_SIZE_MAX);
    strncpy(http_req->uri, uri, URI_SIZE_MAX);
    strncpy(http_req->version, version, VERSION_SIZE_MAX);

    printf("Method: %s\n", http_req->method);
    printf("URI: %s\n", http_req->uri);
    printf("Version: %s\n\n", http_req->version);
}

static void parse_http_request(const char *request_raw, struct Http_request_t *http_req)
{    
    for (size_t i = 0; i < sizeof(http_req->req_line) - 1; i++)
    {
        http_req->req_line[i] = request_raw[i];
        if (i > 0 && request_raw[i - 1] == '\r' && request_raw[i] == '\n') {
            break;
        }
    }
    printf("Request line: %s", http_req->req_line);

    parse_http_req_line(http_req);
}

static int handle_get(struct Http_request_t *http_req, struct Http_response_t *http_resp)
{
    return check_file(http_req->uri, http_resp);
}

void handle_request(const char *request_raw, int sockfd)
{
    struct Http_request_t http_req = { 0 };
    enum Http_method_t method_type;
    struct Http_response_t http_resp = { 0 };
    
    parse_http_request(request_raw, &http_req);

    switch (method_type = parse_http_method(http_req.method))
    {
    case GET:
        http_resp.status_code = handle_get(&http_req, &http_resp);
        break;
    case INVALID:
        http_resp.status_code = 400;
        break;
    default:
        http_resp.status_code = 501;
        break;
    }

    add_response_header(&http_resp, "Test-header", "jeden");
    add_response_header(&http_resp, "Test-header", "dwa");
    add_response_header(&http_resp, "Test-header", "jeden");
    add_response_header(&http_resp, "Test-header", "dwa");

    send_http_response(&http_resp, sockfd);
    free_response(&http_resp);
}
