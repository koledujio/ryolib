#pragma once

#ifdef __OBJC__

#if __has_include(<objc/objc.h>)
#import <objc/objc.h>
#define RYO_OBJC_BOOL_AVAILABLE 1
#endif

#ifndef RYO_OBJC_BOOL_AVAILABLE
typedef signed char BOOL;
#endif
#ifndef YES
#define YES ((BOOL)1)
#endif
#ifndef NO
#define NO ((BOOL)0)
#endif

#ifndef nil
#if defined(__cplusplus)
#define nil nullptr
#else
#define nil ((id)0)
#endif
#endif

#ifndef Nil
#define Nil ((Class)0)
#endif

#endif

__attribute__((objc_root_class))
@interface RYObject {
@private
    Class isa;
    unsigned long _retainCount;
}

+ (instancetype)alloc;
- (instancetype)init;
- (instancetype)retain;
- (void)release;
- (void)dealloc;
- (id)methodSignatureForSelector:(SEL)selector;
- (void)doesNotRecognizeSelector:(SEL)selector;
+ (Class)class;
- (Class)class;

@end
