#import "ryolib/windowing/RYWindow.h"

@implementation RYWindow

@synthesize application = _application;
@synthesize windowId = _windowId;
@synthesize title = _title;
@synthesize flags = _flags;
@synthesize allowedOrientations = _allowedOrientations;
@synthesize internalWindow = _internal;

+ (instancetype)newWindowWithApplication:(RYApplication *)application
                                   title:(RYString *)title
                                   width:(unsigned int)width
                                  height:(unsigned int)height {
    return [[self alloc] initWithApplication:application title:title width:width height:height flags:0 allowedOrientations:0 error:NULL];
}

+ (instancetype)newWindowWithApplication:(RYApplication *)application
                               titleUTF8:(const char *)title
                                   width:(unsigned int)width
                                  height:(unsigned int)height {
    return [[self alloc] initWithApplication:application titleUTF8:title width:width height:height flags:0 allowedOrientations:0 error:NULL];
}

+ (instancetype)newWindowWithApplication:(RYApplication *)application
                                   title:(RYString *)title
                                   width:(unsigned int)width
                                  height:(unsigned int)height
                                   flags:(unsigned int)flags
                     allowedOrientations:(unsigned int)allowedOrientations
                                   error:(RYError **)error {
    return [[self alloc] initWithApplication:application title:title width:width height:height flags:flags allowedOrientations:allowedOrientations error:error];
}

+ (instancetype)newWindowWithApplication:(RYApplication *)application
                               titleUTF8:(const char *)title
                                   width:(unsigned int)width
                                  height:(unsigned int)height
                                   flags:(unsigned int)flags
                     allowedOrientations:(unsigned int)allowedOrientations
                                   error:(RYError **)error {
    return [[self alloc] initWithApplication:application titleUTF8:title width:width height:height flags:flags allowedOrientations:allowedOrientations error:error];
}

- (instancetype)initWithApplication:(RYApplication *)application
                              title:(RYString *)title
                              width:(unsigned int)width
                             height:(unsigned int)height
                              flags:(unsigned int)flags
                allowedOrientations:(unsigned int)allowedOrientations
                              error:(RYError **)error {
    self = [super init];
    if (self) {
        _internal = NULL;
    }
    return self;
}

- (instancetype)initWithApplication:(RYApplication *)application
                          titleUTF8:(const char *)title
                              width:(unsigned int)width
                             height:(unsigned int)height
                              flags:(unsigned int)flags
                allowedOrientations:(unsigned int)allowedOrientations
                              error:(RYError **)error {
    self = [super init];
    if (self) {
        _internal = NULL;
    }
    return self;
}

- (BOOL)isValid { return NO; }
- (void *)nativeWindowHandle { return NULL; }
- (const char *)titleUTF8 { return NULL; }
- (BOOL)setTitleUTF8:(const char *)title { return NO; }
- (BOOL)setTitleUTF8:(const char *)title error:(RYError **)error { return NO; }
- (void)setTitle:(RYString *)title {}

- (RYWindowSize)size { return RYWindowSizeMake(0, 0); }
- (void)setSize:(RYWindowSize)size {}
- (BOOL)getSize:(unsigned int *)outWidth height:(unsigned int *)outHeight { return NO; }
- (BOOL)setSizeWithWidth:(unsigned int)width height:(unsigned int)height { return NO; }
- (BOOL)setSizeWithWidth:(unsigned int)width height:(unsigned int)height error:(RYError **)error { return NO; }

- (RYWindowSize)minSize { return RYWindowSizeMake(0, 0); }
- (void)setMinSize:(RYWindowSize)minSize {}
- (BOOL)getMinSize:(unsigned int *)outWidth height:(unsigned int *)outHeight { return NO; }
- (BOOL)setMinSizeWithWidth:(unsigned int)width height:(unsigned int)height { return NO; }
- (BOOL)setMinSizeWithWidth:(unsigned int)width height:(unsigned int)height error:(RYError **)error { return NO; }

- (RYWindowSize)maxSize { return RYWindowSizeMake(0, 0); }
- (void)setMaxSize:(RYWindowSize)maxSize {}
- (BOOL)getMaxSize:(unsigned int *)outWidth height:(unsigned int *)outHeight { return NO; }
- (BOOL)setMaxSizeWithWidth:(unsigned int)width height:(unsigned int)height { return NO; }
- (BOOL)setMaxSizeWithWidth:(unsigned int)width height:(unsigned int)height error:(RYError **)error { return NO; }

- (float)aspectRatio { return 0.0f; }
- (float)hidpiScale { return 0.0f; }
- (RYSafeAreaInsets)safeAreaInsets { return RYSafeAreaInsetsMake(0.0f, 0.0f, 0.0f, 0.0f); }

- (void)setFlags:(unsigned int)flags {}
- (BOOL)setFlags:(unsigned int)flags error:(RYError **)error { return NO; }

- (void)setAllowedOrientations:(unsigned int)allowedOrientations {}
- (BOOL)setAllowedOrientations:(unsigned int)orientations error:(RYError **)error { return NO; }

- (unsigned int)orientation { return RYDisplayOrientationNone; }

@end
