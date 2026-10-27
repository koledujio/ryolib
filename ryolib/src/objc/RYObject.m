#import "ryolib/objc/RYObject.h"

#include <objc/runtime.h>
#include <stdio.h>
#include <stdlib.h>

@implementation RYObject

+ (instancetype)alloc {
    RYObject *object = (id)class_createInstance(self, 0);
    object->_retainCount = 1;
    return object;
}

- (instancetype)init {
    return self;
}

- (instancetype)retain {
    ++_retainCount;
    return self;
}

- (void)release {
    if (_retainCount == 0 || --_retainCount != 0) {
        return;
    }

    [self dealloc];
}

- (void)dealloc {
    object_dispose(self);
}

- (id)methodSignatureForSelector:(SEL)selector {
    return nil;
}

- (void)doesNotRecognizeSelector:(SEL)selector {
    fprintf(
        stderr,
        "RYObject: %s does not respond to selector '%s'\n",
        class_getName(object_getClass(self)),
        sel_getName(selector)
    );

    abort();
}

+ (Class)class {
    return self;
}

- (Class)class {
    return object_getClass(self);
}

@end
