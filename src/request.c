#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "request.h"
#include "server.h"
#include "socket_comm.h"
#include "utils.h"
#include "response.h"

#define REQ_LINE_SIZE_MAX   150
#define METHOD_SIZE_MAX     25
#define URI_SIZE_MAX        100
#define VERSION_SIZE_MAX    25
#define REQ_FIELD_SIZE_MAX  50
#define REQ_FIELDS_MAX      5
#define MSG_BODY_SIZE_MAX   600

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


static int read_file(char *uri, struct Http_response_t *http_resp)
{   
    char path[sizeof(SERVER_ROOT) + URI_SIZE_MAX] = { 0 };
    convert_uri_to_path(uri, path);

    if (!is_within_root(path))
        return 403;

    const char *modes = "r";
    http_resp->header_field.name = "Content-Type: text/html";
    if (strstr(path, "favicon.ico") != NULL) {
        http_resp->header_field.name = "Content-Type: image/x-icon";
        modes = "rb";
    }
    
    memset(http_resp->msg_body, '\0', sizeof(http_resp->msg_body));
    
    FILE *fp;
    if ((fp = fopen(path, modes)) == NULL) {
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
    else if (!is_file(path)) {
        fclose(fp);
        return 403;
    }
    
    // get the number of bytes
    fseek(fp, 0L, SEEK_END);
    http_resp->msg_body_size = ftell(fp);

    if (http_resp->msg_body_size > (long)sizeof(http_resp->msg_body))
        http_resp->msg_body_size = (long)sizeof(http_resp->msg_body);

    // reset the file position indicator to
    // the beginning of the file
    fseek(fp, 0L, SEEK_SET);

    // copy all the text into the buffer
    fread(http_resp->msg_body, sizeof(char), http_resp->msg_body_size, fp);
    fclose(fp);

    return 200;
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
    const char *req_line = http_req->req_line;
    char *next_element;
    size_t element_size = 0;
    int element_cnt = 0;
    bool req_line_end = false;

    // each element of request line is separated by space,
    // so the element is written into corresponding string when:
    // - space or null occured
    // - element size is greater than zero
    for (size_t i = 0; i < strlen(req_line); i++)
    {
        if (i > 0 && req_line[i - 1] == '\r' && req_line[i] == '\n')
            req_line_end = true;

        if (!isspace(req_line[i]) && element_size == 0)
            next_element = (char *)&req_line[i];
        
        if (!isspace(req_line[i]))
            element_size++;
        
        if ((isspace(req_line[i]) || req_line_end) && element_size > 0) {
            element_cnt++;
            switch (element_cnt)
            {
            case 1:
                if (element_size > METHOD_SIZE_MAX)
                    element_size = METHOD_SIZE_MAX;
                memccpy(http_req->method, next_element, ' ', element_size);
                break;
            case 2:
                // implement 414 req_line-URI Too Long
                if (element_size > URI_SIZE_MAX)
                    element_size = URI_SIZE_MAX;
                memccpy(http_req->uri, next_element, ' ', element_size);
                break;
            case 3:
                if (element_size > VERSION_SIZE_MAX)
                    element_size = VERSION_SIZE_MAX;
                memccpy(http_req->version, next_element, ' ', element_size);
                break;
            default:
                break;
            }
            element_size = 0;
        }
    }
    
    printf("method: %s\n", http_req->method);
    printf("uri: %s\n", http_req->uri);
    printf("version: %s\n\n", http_req->version);
}

static void parse_http_request(char *request_raw, struct Http_request_t *http_req)
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
    return read_file(http_req->uri, http_resp);
}

void handle_request(char *request_raw, int sockfd)
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

    send_http_response(&http_resp, sockfd);
}
