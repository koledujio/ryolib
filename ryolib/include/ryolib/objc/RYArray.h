#pragma once

#include "RYObject.h"

/**
 * A dynamic array of objects.
 * Stores non-owning references to objects. Adding an object doesn't retain it,
 * and removing an object doesn't release it.
 * 
 * Capacity is either zero or a power of two greater than or equal to four.
 */
@interface RYArray : RYObject {
@private
    void *_storage;
}

@property (nonatomic, readonly) unsigned long count;
@property (nonatomic, readonly) unsigned long capacity;

/**
 * Initializes a new array.
 * Doesn't allocate any capacity for objects.
 */
- (instancetype)init;

/**
 * Returns the object at the specified index or nil if the index is out of bounds.
 */
- (id)objectAtIndex:(unsigned long)index;

/**
 * Adds a new object to the array.
 * 
 * If the current capacity is insufficient, the array is reallocated to accommodate the new object.
 * 
 * Returns NO if the object is nil or the operation fails.
 * On failure, the array remains unchanged.
 */
- (BOOL)addObject:(id)object;

/**
 * Removes all occurrances of the specified object.
 * 
 * Doesn't optimize memory automatically.
 * 
 * Returns YES if at least one occurrence was removed, otherwise returns NO.
 */
- (BOOL)removeObject:(id)object;

/**
 * Removes every object from the array.
 * 
 * Doesn't optimize memory automatically.
 */
- (void)removeAllObjects;

/**
 * Ensures the capacity is the smallest allowed capacity to hold all objects.
 * 
 * If the current number of objects is 0, the capacity becomes 0.
 * If the capacity is already optimal, no reallocation occurs.
 * 
 * Returns YES if the capacity is the smallest to hold all objects, or NO if the operation failed.
 * On failure, the array remains unchanged.
 */
- (BOOL)optimize;

/**
 * Ensures the array has sufficient capacity to store the specified number of objects without further allocation.
 * 
 * If the current capacity is sufficient, no reallocation occurs.
 * Otherwise, the capacity is increased to the smallest allowed capacity sufficient to accommodate the requested count.
 * 
 * Returns YES if the capacity is available or successfully allocated, or NO if the operation failed.
 * On failure, the array remains unchanged.
 */
- (BOOL)reserveCapacity:(unsigned long)capacity;

@end
