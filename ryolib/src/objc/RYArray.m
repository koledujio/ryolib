#include "ryolib/objc/RYArray.h"

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
    void **items;
    unsigned long count;
    unsigned long capacity;
} RYArrayStorage;

@implementation RYArray

- (instancetype)init {
    self = [super init];

    if (!self) {
        return nil;
    }

    _storage = calloc(1, sizeof(RYArrayStorage));

    if (!_storage) {
        [self release];
        return nil;
    }

    RYArrayStorage *storage = (RYArrayStorage *)_storage;
    storage->count = 0;
    storage->capacity = 0;

    return self;
}

- (void)dealloc {
    RYArrayStorage *storage = (RYArrayStorage *)_storage;

    if (storage) {
        free(storage->items);
        free(storage);
    }

    [super dealloc];
}

- (unsigned long)count {
    RYArrayStorage *storage = (RYArrayStorage *)_storage;
    return storage->count;
}

- (unsigned long)capacity {
    RYArrayStorage *storage = (RYArrayStorage *)_storage;
    return storage->capacity;
}

- (id)objectAtIndex:(unsigned long)index {
    RYArrayStorage *storage = (RYArrayStorage *)_storage;

    if (index >= storage->count) {
        return nil;
    }

    return (id)storage->items[index];
}

- (BOOL)addObject:(id)object {
    if (!object) {
        return NO;
    }

    RYArrayStorage *storage = (RYArrayStorage *)_storage;
    
    if (storage->count == storage->capacity) {
        unsigned long newCapacity;

        if (storage->capacity == 0) {
            newCapacity = 4;
        } else {
            if (storage->capacity > ULONG_MAX / 2) {
                return NO;
            }

            newCapacity = storage->capacity * 2;
        }

        if (newCapacity > SIZE_MAX / sizeof(void *)) {
            return NO;
        }

        void **newItems = realloc(storage->items, (size_t)newCapacity * sizeof(void *));

        if (!newItems) {
            return NO;
        }

        storage->items = newItems;
        storage->capacity = newCapacity;
    }

    storage->items[storage->count++] = (void *)object;

    return YES;
}

- (BOOL)removeObject:(id)object {
    if (!object) {
        return NO;
    }

    RYArrayStorage *storage = (RYArrayStorage *)_storage;
    
    unsigned long writeIndex = 0;
    unsigned long originalCount = storage->count;

    unsigned long readIndex;
    for (readIndex = 0; readIndex < originalCount; ++readIndex) {
        if ((id)storage->items[readIndex] != object) {
            storage->items[writeIndex++] = storage->items[readIndex];
        }
    }

    storage->count = writeIndex;

    return writeIndex != originalCount;
}

- (void)removeAllObjects {
    RYArrayStorage *storage = (RYArrayStorage *)_storage;
    storage->count = 0;
}

- (BOOL)optimize {
    RYArrayStorage *storage = (RYArrayStorage *)_storage;

    unsigned long optimalCapacity = 0;

    if (storage->count > 0) {
        optimalCapacity = 4;

        while (optimalCapacity < storage->count) {
            if (optimalCapacity > ULONG_MAX / 2) {
                return NO;
            }

            optimalCapacity *= 2;
        }
    }

    if (storage->capacity == optimalCapacity) {
        return YES;
    }

    if (optimalCapacity == 0) {
        free(storage->items);
        storage->items = NULL;
        storage->capacity = 0;
        return YES;
    }

    if (optimalCapacity > SIZE_MAX / sizeof(void *)) {
        return NO;
    }

    void **newItems = realloc(storage->items, (size_t)optimalCapacity * sizeof(void *));

    if (!newItems) {
        return NO;
    }

    storage->items = newItems;
    storage->capacity = optimalCapacity;

    return YES;
}

- (BOOL)reserveCapacity:(unsigned long)capacity {
    RYArrayStorage *storage = (RYArrayStorage *)_storage;

    if (capacity <= storage->capacity) {
        return YES;
    }

    unsigned long newCapacity = 4;

    while (newCapacity < capacity) {
        if (newCapacity > ULONG_MAX / 2) {
            return NO;
        }

        newCapacity *= 2;
    }

    if (newCapacity > SIZE_MAX / sizeof(void *)) {
        return NO;
    }

    void **newItems = realloc(storage->items, (size_t)newCapacity * sizeof(void *));

    if (!newItems) {
        return NO;
    }

    storage->items = newItems;
    storage->capacity = newCapacity;

    return YES;
}

@end
