#ifndef MIME_H
#define MIME_H

typedef struct {
    const char *ext;
    const char *type;
    const char *read_mode;
} mime_t;

static const mime_t mime_type_map[] = {
    { "html",   "text/html",    "r" },
    { "ico",    "image/x-icon", "rb" },
    { 0, 0, 0 }
};

static const mime_t mime_type_default = {
    "html",   "text/html",    "r"
};

mime_t get_mime_type(const char *path);

#endif