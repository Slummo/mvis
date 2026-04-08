#pragma once

#include <stdio.h>
#include <errno.h>

#define ERR(fmt, ...)                                                       \
    do {                                                                    \
        fprintf(stderr, "[%s:%d] " fmt, __FILE__, __LINE__, ##__VA_ARGS__); \
        if (errno != 0) {                                                   \
            perror(". ERR");                                                \
            errno = 0;                                                      \
        } else {                                                            \
            fprintf(stderr, "\n");                                          \
        }                                                                   \
    } while (0);
