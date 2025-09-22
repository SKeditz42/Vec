#include "vec.h"

Vec vec_new(size_t elem_size)
{
    Vec v =
    {
        .data=NULL,
        .len=0,
        .capacity=0,
        .elem_size=elem_size,
        #ifndef LITE
            .push=vec_push,
            .pop=vec_pop,
            .shrink=vec_shrink,
            .at=vec_at,
            .destroy=vec_destroy,
            .reserve=vec_reserve,
            .clear=vec_clear,
            .back=vec_back,
            .remove=vec_remove,
            .begin=vec_begin,
            .end=vec_end,
        #endif
    };
    return v;
}

void *vec_push(Vec *v, const void *elem)
{
    if (v->len == v->capacity)
    {
        size_t new_cap;
        if (v->capacity == 0)
        {
            new_cap = 1;
        }
        else if (v->elem_size != 0 && v->capacity > SIZE_MAX / 2 / v->elem_size)
        {
            errno = ENOMEM;
            perror("push capacity overflow");
            return NULL;
        }
        else if (v->elem_size == 0 && v->capacity > SIZE_MAX / 2)
        {
            new_cap = 0;
        }
        else
        {
            new_cap = v->capacity * 2;
        }

        if (v->elem_size != 0 && new_cap > SIZE_MAX / v->elem_size)
        {
            errno = ENOMEM;
            perror("push capacity overflow");
            return NULL;
        }

        void *tmp = realloc(v->data, new_cap * v->elem_size);
        if (!tmp)
        {
            errno = ENOMEM;
            perror("push realloc");
            return NULL;
        }

        v->data = tmp;
        v->capacity = new_cap;
    }

    void *dest = memcpy(v->data + v->len * v->elem_size, elem, v->elem_size);

    v->len++;

    return dest;
}

void vec_pop(Vec *v)
{
    if (v->len > 0)
    {
        v->len--;
    }

    return;
}

void vec_shrink(Vec *v)
{
    if (v->len == 0)
    {
        free(v->data);
        v->data = NULL;
        v->capacity = 0;

        return;
    }

    void *tmp = realloc(v->data, v->len * v->elem_size);
    if (!tmp)
    {
        perror("shrink realloc");
        return;
    }

    v->data = tmp;
    v->capacity = v->len;

    return;
}

void *vec_at(Vec *v, size_t i)
{
    if (i >= v->len || v->data == NULL)
    {
        return NULL;
    }

    return v->data + i * v->elem_size;
}

void vec_destroy(Vec *v)
{
    free(v->data);
    v->data = NULL;
    v->len = 0;
    v->capacity = 0;
    v->elem_size = 0;

    return;
}

int vec_reserve(Vec *v, size_t new_capacity)
{
    if (new_capacity <= v->capacity)
    {
        return 1;
    }

    if (v->elem_size != 0 && new_capacity > SIZE_MAX / v->elem_size)
    {
        errno = ENOMEM;
        perror("reserve capacity overflow");
        return 0;
    }

    void *tmp = realloc(v->data, new_capacity * v->elem_size);
    if (!tmp)
    {
        errno = ENOMEM;
        perror("reserve realloc");
        return 0;
    }

    v->data = tmp;
    v->capacity = new_capacity;

    return 1;
}

void vec_clear(Vec *v)
{
    v->len = 0;
}

void *vec_back(Vec *v)
{
    if (v->len == 0 || v->data == NULL)
    {
        return NULL;
    }

    return v->data + (v->len - 1) * v->elem_size;
}

int vec_remove(Vec *v, size_t i)
{
    if (i >= v->len)
    {
        return 0;
    }

    if (i < v->len - 1)
    {
        unsigned char *dst = v->data + i * v->elem_size;
        unsigned char *src = v->data + (i + 1) * v->elem_size;
        size_t bytes = (v->len - i - 1) * v->elem_size;
        memmove(dst, src, bytes);
    }

    v->len--;
    
    return 1;
}

void* vec_begin(Vec *v)
{
    return v->data;
}

void* vec_end(Vec *v)
{
    if (v->data == NULL) return NULL;
    
    return v->data + v->len * v->elem_size;
}
