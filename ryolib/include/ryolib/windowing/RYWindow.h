#pragma once

#import "../objc/RYObject.h"
#import "../objc/RYString.h"
#import "../objc/RYError.h"
#import "RYWindowingTypes.h"
#import "RYApplication.h"

/**
 * @brief Represents a platform native window.
 */
@interface RYWindow : RYObject {
@protected
    RYApplication *_application;
    RYString *_title;
    RYWindowID _windowId;
    unsigned int _flags;
    unsigned int _allowedOrientations;
    void *_internal;
}

@property (nonatomic, readonly) RYApplication *application;
@property (nonatomic, readonly) BOOL isValid;
@property (nonatomic, readonly) RYWindowID windowId;
@property (nonatomic, readonly) void *nativeWindowHandle;
@property (nonatomic, readonly) void *internalWindow;

@property (nonatomic, retain) RYString *title;
@property (nonatomic, readonly) const char *titleUTF8;
- (BOOL)setTitleUTF8:(const char *)title;
- (BOOL)setTitleUTF8:(const char *)title error:(RYError **)error;

@property (nonatomic) RYWindowSize size;
- (BOOL)getSize:(unsigned int *)outWidth height:(unsigned int *)outHeight;
- (BOOL)setSizeWithWidth:(unsigned int)width height:(unsigned int)height;
- (BOOL)setSizeWithWidth:(unsigned int)width height:(unsigned int)height error:(RYError **)error;

@property (nonatomic) RYWindowSize minSize;
- (BOOL)getMinSize:(unsigned int *)outWidth height:(unsigned int *)outHeight;
- (BOOL)setMinSizeWithWidth:(unsigned int)width height:(unsigned int)height;
- (BOOL)setMinSizeWithWidth:(unsigned int)width height:(unsigned int)height error:(RYError **)error;

@property (nonatomic) RYWindowSize maxSize;
- (BOOL)getMaxSize:(unsigned int *)outWidth height:(unsigned int *)outHeight;
- (BOOL)setMaxSizeWithWidth:(unsigned int)width height:(unsigned int)height;
- (BOOL)setMaxSizeWithWidth:(unsigned int)width height:(unsigned int)height error:(RYError **)error;

@property (nonatomic, readonly) float aspectRatio;
@property (nonatomic, readonly) float hidpiScale;
@property (nonatomic, readonly) RYSafeAreaInsets safeAreaInsets;

@property (nonatomic) unsigned int flags;
- (BOOL)setFlags:(unsigned int)flags error:(RYError **)error;

@property (nonatomic) unsigned int allowedOrientations;
- (BOOL)setAllowedOrientations:(unsigned int)allowedOrientations error:(RYError **)error;

@property (nonatomic, readonly) unsigned int orientation;

+ (instancetype)newWindowWithApplication:(RYApplication *)application
                                   title:(RYString *)title
                                   width:(unsigned int)width
                                  height:(unsigned int)height;

+ (instancetype)newWindowWithApplication:(RYApplication *)application
                               titleUTF8:(const char *)title
                                   width:(unsigned int)width
                                  height:(unsigned int)height;

+ (instancetype)newWindowWithApplication:(RYApplication *)application
                                   title:(RYString *)title
                                   width:(unsigned int)width
                                  height:(unsigned int)height
                                   flags:(unsigned int)flags
                     allowedOrientations:(unsigned int)allowedOrientations
                                   error:(RYError **)error;

+ (instancetype)newWindowWithApplication:(RYApplication *)application
                               titleUTF8:(const char *)title
                                   width:(unsigned int)width
                                  height:(unsigned int)height
                                   flags:(unsigned int)flags
                     allowedOrientations:(unsigned int)allowedOrientations
                                   error:(RYError **)error;

- (instancetype)initWithApplication:(RYApplication *)application
                              title:(RYString *)title
                              width:(unsigned int)width
                             height:(unsigned int)height
                              flags:(unsigned int)flags
                allowedOrientations:(unsigned int)allowedOrientations
                              error:(RYError **)error;

- (instancetype)initWithApplication:(RYApplication *)application
                          titleUTF8:(const char *)title
                              width:(unsigned int)width
                             height:(unsigned int)height
                              flags:(unsigned int)flags
                allowedOrientations:(unsigned int)allowedOrientations
                              error:(RYError **)error;

@end
