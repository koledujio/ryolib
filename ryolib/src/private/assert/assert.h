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

#elifdef RYO_SAFETY

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