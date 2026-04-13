#pragma once

#include <stdio.h>
#include <stdlib.h>

#define da_define(type)        \
    typedef struct type##_da { \
        type* arr;             \
        size_t len;            \
        size_t cap;            \
    } type##_da;

#define da_init(da, initial_cap)                            \
    do {                                                    \
        (da).arr = malloc(sizeof(*(da).arr) * initial_cap); \
        (da).len = 0;                                       \
        (da).cap = initial_cap;                             \
    } while (0)

#define da_len(da) ((da).len)
#define da_cap(da) ((da).cap)
#define da_isnull(da) ((da).arr == NULL)
#define da_isempty(da) (da_len(da) == 0)
#define da_get(da, i) ((da).arr[i])

#define da_append(da, item)                                             \
    do {                                                                \
        if ((da).len >= (da).cap) {                                     \
            (da).cap = (da).cap == 0 ? 1 : (da).cap << 1;               \
            (da).arr = realloc((da).arr, sizeof(*(da).arr) * (da).cap); \
        }                                                               \
        (da).arr[(da).len++] = item;                                    \
    } while (0)

#define da_pop(da)                                   \
    do {                                             \
        (da).len = (da).len == 0 ? 0 : (da).len - 1; \
    } while (0)

#define da_clear(da)  \
    do {              \
        (da).len = 0; \
    } while (0)

#define da_reserve(da, n)                                    \
    do {                                                     \
        (da).arr = realloc((da).arr, sizeof(*(da).arr) * n); \
    } while (0)

#define da_free(da)     \
    do {                \
        free((da).arr); \
    } while (0)
