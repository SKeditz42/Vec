#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

static int test_total = 0;
static int test_failed = 0;

#define EXPECT_TRUE(cond) do { \
    ++test_total; \
    if (!(cond)) { \
        ++test_failed; \
        fprintf(stderr, "EXPECT_TRUE failed: %s at %s:%d\n", #cond, __FILE__, __LINE__); \
    } \
} while (0)

#define EXPECT_EQ_ULL(a,b) do { \
    unsigned long long _va = (unsigned long long)(a); \
    unsigned long long _vb = (unsigned long long)(b); \
    ++test_total; \
    if (_va != _vb) { \
        ++test_failed; \
        fprintf(stderr, "EXPECT_EQ failed: %s (%llu) != %s (%llu) at %s:%d\n", #a, _va, #b, _vb, __FILE__, __LINE__); \
    } \
} while (0)

#define EXPECT_EQ_INT(a,b) do { \
    long long _va = (long long)(a); \
    long long _vb = (long long)(b); \
    ++test_total; \
    if (_va != _vb) { \
        ++test_failed; \
        fprintf(stderr, "EXPECT_EQ failed: %s (%lld) != %s (%lld) at %s:%d\n", #a, _va, #b, _vb, __FILE__, __LINE__); \
    } \
} while (0)

#define EXPECT_MEMEQ(a,b,n) do { \
    ++test_total; \
    if (memcmp((a),(b),(n)) != 0) { \
        ++test_failed; \
        fprintf(stderr, "EXPECT_MEMEQ failed: %s != %s (%zu bytes) at %s:%d\n", #a, #b, (size_t)(n), __FILE__, __LINE__); \
    } \
} while (0)

static inline int test_summary(const char *suite) {
    if (test_failed == 0) {
        printf("[OK] %s: %d checks\n", suite, test_total);
        return 0;
    } else {
        printf("[FAIL] %s: %d/%d failed\n", suite, test_failed, test_total);
        return 1;
    }
}

static inline void run_test(const char *name, void (*fn)(void)) {
    int before_failed = test_failed;
    int before_total  = test_total;
    fn();
    int df = test_failed - before_failed;
    int dt = test_total  - before_total;
    if (df == 0) {
        printf("[OK] %s: %d checks\n", name, dt);
    } else {
        printf("[FAIL] %s: %d/%d failed\n", name, df, dt);
    }
}

#define RUN_TEST(fn) run_test(#fn, fn)

#endif /* TEST_H */
