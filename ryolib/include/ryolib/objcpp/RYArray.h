#pragma once

#include "RYObject.h"

@interface RYArray : RYObject {
@private
    void *_storage;
}

- (instancetype)init;
- (unsigned long)count;
- (id)objectAtIndex:(unsigned long)index;
- (void)addObject:(id)object;
- (void)removeObject:(id)object;
- (void)removeAllObjects;

@end
