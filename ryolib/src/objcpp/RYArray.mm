#include "ryolib/objcpp/RYArray.h"

#include <algorithm>
#include <stdexcept>
#include <vector>

struct RYArrayStorage {
    std::vector<void *> items;
};

@implementation RYArray

- (instancetype)init {
    self = [super init];

    if (self) {
        _storage = new RYArrayStorage{};
    }

    return self;
}

- (void)dealloc {
    delete static_cast<RYArrayStorage *>(_storage);
    [super dealloc];
}

- (unsigned long)count {
    auto *storage = static_cast<RYArrayStorage *>(_storage);
    return static_cast<unsigned long>(storage->items.size());
}

- (id)objectAtIndex:(unsigned long)index {
    auto *storage = static_cast<RYArrayStorage *>(_storage);

    if (index >= storage->items.size()) {
        throw std::out_of_range("RYArray index out of range");
    }

    return static_cast<id>(storage->items[index]);
}

- (void)addObject:(id)object {
    if (!object) {
        throw std::invalid_argument("RYArray cannot contain nil");
    }

    auto *storage = static_cast<RYArrayStorage *>(_storage);
    storage->items.push_back(static_cast<void *>(object));
}

- (void)removeObject:(id)object {
    auto *storage = static_cast<RYArrayStorage *>(_storage);
    auto &items = storage->items;

    items.erase(std::remove(items.begin(), items.end(), static_cast<void *>(object)), items.end());
}

- (void)removeAllObjects {
    auto *storage = static_cast<RYArrayStorage *>(_storage);
    storage->items.clear();
}

@end
