#include <string.h>

#include "mime.h"

mime_t get_mime_type(const char *path)
{
    const char *last_dot = strrchr(path, '.');
    const char *ext = last_dot ? last_dot + 1 : mime_type_default.ext;
    
    const mime_t *mime_type;

    // iterate through mime_type_map until finding record
    // with the same ext as input path extension
    for (mime_type = mime_type_map; mime_type->type; mime_type++)
        if (strcmp(ext, mime_type->ext) == 0)
            break;

    // if no match found, apply default one
    if (mime_type->type == 0)
        mime_type = &mime_type_default;
    
    return *mime_type;
}