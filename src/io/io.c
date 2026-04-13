#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* file_content(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        return NULL;
    }

    fseek(f, 0l, SEEK_END);
    size_t size = (size_t)ftell(f);
    rewind(f);

    char* content = malloc(size + 1);
    if (!content) {
        fclose(f);
        return NULL;
    }

    fread(content, 1, size, f);
    content[size] = '\0';

    fclose(f);

    return content;
}