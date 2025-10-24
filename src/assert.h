#pragma once

#ifdef NDEBUG
#define assert(expr) ((void) 0)
#else
#define assert(expr)                                          \
        if (!(expr)) {                                        \
                panic("assertion failed: %s:%s:%d\nexpr: %s", \
                      __FILE__,                               \
                      __func__,                               \
                      __LINE__,                               \
                      #expr);                                 \
        }

#define static_assert(cond, msg) \
        typedef char static_assertion_##msg[2 * (!!(cond)) - 1]

#endif
