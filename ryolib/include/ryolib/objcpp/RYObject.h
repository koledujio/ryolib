#pragma once

#ifdef __OBJC__

#ifndef nil
#define nil ((id)0)
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
