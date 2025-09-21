#ifndef VEC_H
#define VEC_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

#ifndef SIZE_MAX
    #define SIZE_MAX ((size_t)-1)
#endif


typedef struct Vec
{
    unsigned char *data;

    size_t len;
    size_t capacity;
    size_t elem_size;

#ifndef LITE
    void* (*push)   (struct Vec *v, const void *elem);
    void  (*pop)    (struct Vec *v);
    void  (*shrink) (struct Vec *v);
    void* (*at)     (struct Vec *v, size_t i);
    void  (*destroy)(struct Vec *v);
    int   (*reserve)(struct Vec *v, size_t new_capacity);
    void  (*clear)  (struct Vec *v);
    void* (*back)   (struct Vec *v);
    int   (*remove) (struct Vec *v, size_t i);
    void* (*begin)  (struct Vec *v);
    void* (*end)    (struct Vec *v);
#endif

} Vec;


/* Returns pointer to inserted slot on success, or NULL on failure.
 * On allocation failure or capacity overflow, errno is set to ENOMEM and
 * the vector remains unchanged.
 */
Vec   vec_new(size_t elem_size);
void* vec_push(Vec *v, const void *elem);
void  vec_pop(Vec *v);
void  vec_shrink(Vec *v);
void* vec_at(Vec *v, size_t i);
int   vec_reserve(Vec *v, size_t new_capacity);
void  vec_clear(Vec *v);
void* vec_back(Vec *v);
int   vec_remove(Vec *v, size_t i);
void* vec_begin(Vec *v);
void* vec_end(Vec *v);
void  vec_destroy(Vec *v);

#endif /* VEC_H */
