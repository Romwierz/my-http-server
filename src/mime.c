#include <string.h>

#include "mime.h"

mime_t get_mime_type(const char *path)
{
    const mime_t *mime_type;

    // iterate through mime_type_map until finding record
    // with the same ext as input path extension
    for (mime_type = mime_type_map; mime_type->type; mime_type++)
        if (strstr(path, mime_type->ext) != NULL)
            break;

    // if no match found, apply default one
    if (mime_type->type == 0)
        mime_type = &mime_type_default;
    
    return *mime_type;
}