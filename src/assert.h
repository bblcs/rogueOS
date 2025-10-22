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

#endif
