/*
    Assertion and safety macros.

    Which checks are compiled in is decided by the RYO_DEBUG and RYO_SAFETY definitions:

    - RYO_DEBUG: enable this at all times while developing. Every check is compiled in
      and aborts the process when an invalid use is detected, so mistakes surface where
      they are made instead of turning into corrupt frames or crashes somewhere later.
    - RYO_SAFETY: use this when something crashes or behaves invalidly and more checks are
      needed to find out why, it performs the same checks as RYO_DEBUG but sets an error
      and returns instead of aborting, which prevents crashes at the cost of performance.
    - Neither: for release builds. All checks are compiled out, so every precondition has
      to be guaranteed by the caller. Use this only when following the guidelines exactly
      is guaranteed, not as a way to make problems go away.

    RYO_ASSERT_VOID, RYO_ASSERT and RYO_ASSERT_NORETURN check conditions that must never
    happen, they abort while developing and are compiled out otherwise.
    RYO_ENSURE_VOID and RYO_ENSURE check the preconditions of the api, they abort while
    developing, set an error and return while RYO_SAFETY is enabled and are compiled out
    otherwise.
*/
#pragma once

#include <cstdio>

#include "ryolib/debug.h"

#if defined(_MSC_VER)
    #define RYO_DEBUG_BREAK() __debugbreak()
#elif defined(__clang__) || defined(__GNUC__)
    #define RYO_DEBUG_BREAK() __builtin_trap()
#else
    #define RYO_DEBUG_BREAK() abort()
#endif


#ifdef RYO_DEBUG

#define RYO_ASSERT_VOID(expr, msg) \
    do { \
        if (!(expr)) { \
            printf("!!! Ryo Assert Triggered !!!\nFILE: %s\nLINE: %d\nEXPRESSION: %s\nMESSAGE: %s\n", __FILE__, __LINE__, #expr, msg); \
            RYO_DEBUG_BREAK(); \
            return; \
        } \
    } while (0)

#define RYO_ASSERT(expr, ret, msg) \
    do { \
        if (!(expr)) { \
            printf("!!! Ryo Assert Triggered !!!\nFILE: %s\nLINE: %d\nEXPRESSION: %s\nMESSAGE: %s\n", __FILE__, __LINE__, #expr, msg); \
            RYO_DEBUG_BREAK(); \
            return ret; \
        } \
    } while (0)

#define RYO_ASSERT_NORETURN(expr, msg) \
    do { \
        if (!(expr)) { \
            printf("!!! Ryo Assert Triggered !!!\nFILE: %s\nLINE: %d\nEXPRESSION: %s\nMESSAGE: %s\n", __FILE__, __LINE__, #expr, msg); \
            RYO_DEBUG_BREAK(); \
        } \
    } while (0)

#else

#define RYO_ASSERT_VOID(expr, msg) ((void)0)
#define RYO_ASSERT(expr, ret, msg) ((void)0)
#define RYO_ASSERT_NORETURN(expr, msg) ((void)0)

#endif

#ifdef RYO_DEBUG

#define RYO_ENSURE_VOID(expr, msg) \
    do { \
        if (!(expr)) { \
            printf("!!! Ryo Safety Violation !!!\nFILE: %s\nLINE: %d\nEXPRESSION: %s\nMESSAGE: %s\n", __FILE__, __LINE__, #expr, msg); \
            RYO_DEBUG_BREAK(); \
            return; \
        } \
    } while(0)

#define RYO_ENSURE(expr, ret, msg) \
    do { \
        if (!(expr)) { \
            printf("!!! Ryo Safety Violation !!!\nFILE: %s\nLINE: %d\nEXPRESSION: %s\nMESSAGE: %s\n", __FILE__, __LINE__, #expr, msg); \
            RYO_DEBUG_BREAK(); \
            return ret; \
        } \
    } while(0)

#elif defined(RYO_SAFETY)

#define RYO_ENSURE_VOID(expr, msg) \
    do { \
        if (!(expr)) { \
            error_set(msg); \
            return; \
        } \
    } while (0)

#define RYO_ENSURE(expr, ret, msg) \
    do { \
        if (!(expr)) { \
            error_set(msg); \
            return ret; \
        } \
    } while (0)

#else

#define RYO_ENSURE_VOID(expr, msg) ((void)0)
#define RYO_ENSURE(expr, ret, msg) ((void)0)

#endif