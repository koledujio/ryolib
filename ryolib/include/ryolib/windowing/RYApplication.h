#pragma once

#import "../objc/RYObject.h"
#import "../objc/RYError.h"
#import "RYWindowingTypes.h"

/**
 * @brief Manages application initialization, platform capabilities, and system event polling.
 */
@interface RYApplication : RYObject {
@protected
    BOOL _isValid;
    RYPlatformCapabilities _capabilities;
    void *_internal;
}

@property (nonatomic, readonly) BOOL isValid;
@property (nonatomic, readonly) RYPlatformCapabilities capabilities;
@property (nonatomic, readonly) void *internalApplication;

+ (instancetype)newApplication;
+ (instancetype)newApplicationWithError:(RYError **)error;

- (instancetype)init;
- (instancetype)initWithError:(RYError **)error;

/**
 * @brief Polls the next event from the application event queue.
 *
 * @param event Pointer to an RYEvent structure to populate.
 * @return YES if an event was retrieved, NO if queue is empty or application is invalid.
 */
- (BOOL)pollEvent:(RYEvent *)event;

@end
