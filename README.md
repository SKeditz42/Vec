# Vec — A Generic Vector for C

Vec is a generic, fast, leak‑safe dynamic array for C. It stores elements contiguously, grows geometrically (×2) for amortized O(1) push, and offers a method‑style API that feels natural if you like object syntax in C. The library is defensive by default: overflow guards before allocations, bounds‑checked accessors, and well‑defined behavior for empty/shrink/destroy.

Why you might want it
- Contiguous memory: better cache locality and prefetching than node‑based structures, fewer pointer indirections, great for iteration and SIMD‑friendly data.
- Geometric growth: doubles capacity at the edge, so most pushes are O(1). Resizes are infrequent and predictable.
- Truly generic in C: type erasure via `elem_size` — no macros, no code generation, no void* arithmetic in user code.
- Ergonomic API: free‑function API in all builds; optional method‑style function pointers if you like object syntax in C; `begin`/`end` for clean for‑loops.
- Safety first: guards against `size_t` overflow on capacity math, safe `shrink(0)`, `pop` is a no‑op on empty, `vec_at` returns NULL out‑of‑bounds, `destroy` zeros state. The test suite runs clean under valgrind.

Design in a nutshell
- Contiguous storage: elements live in a single growable buffer. This maximizes spatial locality and enables tight pointer iteration: `for (T* it = begin; it != end; ++it) { ... }`.
- Growth strategy: capacity grows by ×2 when needed. The classic amortized analysis shows O(1) average push because the cost of copy during reallocation is spread across many future inserts.
- Size vs capacity: `len` is the number of elements; `capacity` is how many fit without reallocating. Use `vec_reserve` to pre‑grow, and `vec_shrink` to release slack back to the allocator.
- Robust realloc: `vec_shrink` handles `len == 0` by freeing and nulling the buffer (no dangling pointer from `realloc(ptr, 0)`). All allocation paths guard against `SIZE_MAX / elem_size` overflow before multiplying.
- Predictable pointers: pointers from `vec_at`, `vec_begin`, `vec_end`, and `vec_back` are stable until a resizing operation (`push` that grows, `reserve`, `shrink`). After a grow/shrink, reacquire pointers.

Memory model (what you can rely on)
- The backing storage is a single `unsigned char*` block. Elements are laid out back‑to‑back with stride `elem_size`.
- Iteration uses standard pointer arithmetic over your element type: `T* it = (T*)vec_begin(&v); for (; it != (T*)vec_end(&v); ++it) ...` (method‑style variants exist too).
- Because the data is contiguous, bulk ops like `memcpy`/`memmove` for remove/shrink are straightforward and efficient.

Build, Install, Test
```
# Build static library in ./build
make

# Install header and library (default PREFIX=/usr/local)
sudo make install

# Run tests (links against installed lib, uses <vec.h>)
make test
```

Linking in your project
```
cc -std=c11 -O2 your_app.c -lvec -o your_app
```

Compile‑time option
- LITE mode (no method pointers in `Vec`):
  - Build the library with `-DLITE` to minimize the `Vec` footprint and expose only the free‑function API.
  - Example: `CFLAGS="-std=c11 -O2 -DLITE" make clean && make`

Usage examples

Free‑function style (works in all builds):
```c
#include <vec.h>
#include <stdio.h>

int main(void) {
  Vec v = vec_new(sizeof(int));
  for (int i = 1; i <= 5; ++i) vec_push(&v, &i);

  for (int *it = (int*)vec_begin(&v); it != (int*)vec_end(&v); ++it) {
    printf("%d ", *it);
  }
  printf("| at[0]=%d | back=%d\n",
         *(int*)vec_at(&v, 0),
         *(int*)vec_back(&v));

  vec_destroy(&v);
  return 0;
}
```

Method‑style (requires non‑LITE build):
```c
#include <vec.h>
#include <stdio.h>

int main(void) {
  Vec v = vec_new(sizeof(char));
  const char s[] = "abcdef";
  for (size_t i = 0; i < sizeof(s)-1; ++i) v.push(&v, &s[i]);

  for (char *it = v.begin(&v); it != v.end(&v); ++it) printf("%c ", *it);
  printf("| at[1]=%c | back=%c\n",
         *(char*)v.at(&v, 1),
         *(char*)v.back(&v));

  v.destroy(&v);
  return 0;
}
```

Safety & semantics
- Allocation safety: capacity growth checks avoid `size_t` overflow; on failure, operations return `NULL`/`0` and set `errno = ENOMEM`.
- `vec_at(v,i)`: returns `NULL` when `i >= len` or data is `NULL`.
- `vec_pop(v)`: no‑op when empty.
- `vec_shrink(v)`: if `len == 0`, frees storage and sets `data=NULL`, `capacity=0`.
- `vec_destroy(v)`: frees and resets all fields to a safe, zero state.

Complexity
- `push`: amortized O(1) with ×2 growth.
- `reserve`: O(n) only when it must reallocate; O(1) otherwise.
- `remove(i)`: O(n − i) due to tail shift.
- `at`, `back`, `begin`, `end`, `clear`, `pop`: O(1).

API reference

Types
```c
typedef struct Vec {
  unsigned char *data;
  size_t len;
  size_t capacity;
  size_t elem_size;

#ifndef LITE
  void* (*push)   (struct Vec*, const void*);
  void  (*pop)    (struct Vec*);
  void  (*shrink) (struct Vec*);
  void* (*at)     (struct Vec*, size_t);
  void  (*destroy)(struct Vec*);
  int   (*reserve)(struct Vec*, size_t);
  void  (*clear)  (struct Vec*);
  void* (*back)   (struct Vec*);
  int   (*remove) (struct Vec*, size_t);
  void* (*begin)  (struct Vec*);
  void* (*end)    (struct Vec*);
#endif
} Vec;
```

Constructors & memory
- `Vec vec_new(size_t elem_size);`
  - New empty vector for elements of `elem_size` bytes.
- `void vec_destroy(Vec* v);`
  - Free internal storage and reset fields to a zero/NULL state.

Capacity & size
- `int vec_reserve(Vec* v, size_t new_capacity);`
  - Ensure capacity ≥ `new_capacity`. Returns 1 on success, 0 on failure (errno=ENOMEM).
- `void vec_shrink(Vec* v);`
  - Shrink allocation to exactly `len`. If `len==0`, free storage and set `capacity=0`.
- `void vec_clear(Vec* v);`
  - Set `len=0` without freeing capacity.

Element access & modification
- `void* vec_push(Vec* v, const void* elem);`
  - Append a copy of `*elem` (of size `elem_size`). Returns pointer to inserted slot, or `NULL` (errno=ENOMEM).
- `void vec_pop(Vec* v);`
  - Remove last element if any; no‑op when empty.
- `int vec_remove(Vec* v, size_t i);`
  - Remove element at `i` (shifts tail). Returns 1 on success, 0 if `i >= len`.
- `void* vec_at(Vec* v, size_t i);`
  - Pointer to element at `i`, or `NULL` when out‑of‑bounds.
- `void* vec_back(Vec* v);`
  - Pointer to last element, or `NULL` when empty.

Iteration
- `void* vec_begin(Vec* v);`
  - Pointer to the first element, or `NULL` when empty.
- `void* vec_end(Vec* v);`
  - One‑past‑the‑end pointer (safe to compare, not to dereference). When `len==0`, may be `NULL`.

Method‑style (non‑LITE)
- The same operations are exposed as function pointers on `Vec` when built without `-DLITE`.
  - Example: `v.push(&v, &elem);` and `for (T* it = v.begin(&v); it != v.end(&v); ++it) ...`

Portability
- Uses `SIZE_MAX` from `<stdint.h>` (C99+). Tested with standard libcs on Linux/macOS.