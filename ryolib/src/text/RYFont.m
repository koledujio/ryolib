#import "ryolib/text/RYFont.h"
#import "ryolib/image/RYImage.h"
#import "sdl_ttf/TextBackendSDL.h"
#import "stub/TextBackendStub.h"

@implementation RYFont

@synthesize path = _path;
@synthesize fontSize = _fontSize;
@synthesize fallbackFonts = _fallbackFonts;
@synthesize backendInstance = _backendInstance;
@synthesize backendFont = _backendFont;

+ (Class)backendClassForType:(RYTextBackendType)type {
    switch (type) {
        case RYTextBackendTypeDefault:
#if defined(RYO_ENABLE_TEXT_SDL)
            return [RYTextBackendSDL class];
#else
            return [RYTextBackendStub class];
#endif
        case RYTextBackendTypeSDL3TTF:
#if defined(RYO_ENABLE_TEXT_SDL)
            return [RYTextBackendSDL class];
#else
            return Nil;
#endif
        case RYTextBackendTypeStub:
            return [RYTextBackendStub class];
        default:
            return Nil;
    }
}

+ (instancetype)newFontWithPath:(RYString *)path size:(float)fontSize {
    return [self newFontWithPath:path size:fontSize backend:RYTextBackendTypeDefault error:NULL];
}

+ (instancetype)newFontWithPath:(RYString *)path size:(float)fontSize error:(RYError **)error {
    return [self newFontWithPath:path size:fontSize backend:RYTextBackendTypeDefault error:error];
}

+ (instancetype)newFontWithPath:(RYString *)path
                           size:(float)fontSize
                        backend:(RYTextBackendType)backendType
                          error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (!path) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeTextInvalidFontPath
                                           message:"Font path is nil"];
        }
        return nil;
    }

    Class backendClass = [self backendClassForType:backendType];
    if (backendClass == Nil) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeTextInvalidBackend
                                           message:"Requested text backend is unsupported or invalid"];
        }
        return nil;
    }

    RYTextBackend *backend = [[backendClass alloc] initBackendWithError:error];
    if (!backend) {
        return nil;
    }

    void *backendFont = [backend createFontWithPath:path fontSize:fontSize error:error];
    if (!backendFont) {
        [backend release];
        return nil;
    }

    RYFont *font = [[self alloc] initWithPath:path
                                     fontSize:fontSize
                              backendInstance:backend
                                  backendFont:backendFont];
    [backend release];
    return font;
}

+ (instancetype)newFontWithMemory:(const void *)memory size:(unsigned long)size fontSize:(float)fontSize {
    return [self newFontWithMemory:memory size:size fontSize:fontSize backend:RYTextBackendTypeDefault error:NULL];
}

+ (instancetype)newFontWithMemory:(const void *)memory size:(unsigned long)size fontSize:(float)fontSize error:(RYError **)error {
    return [self newFontWithMemory:memory size:size fontSize:fontSize backend:RYTextBackendTypeDefault error:error];
}

+ (instancetype)newFontWithMemory:(const void *)memory
                             size:(unsigned long)size
                         fontSize:(float)fontSize
                          backend:(RYTextBackendType)backendType
                            error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (!memory || size == 0) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeTextInvalidMemory
                                           message:"Memory pointer is null or size is zero"];
        }
        return nil;
    }

    Class backendClass = [self backendClassForType:backendType];
    if (backendClass == Nil) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeTextInvalidBackend
                                           message:"Requested text backend is unsupported or invalid"];
        }
        return nil;
    }

    RYTextBackend *backend = [[backendClass alloc] initBackendWithError:error];
    if (!backend) {
        return nil;
    }

    void *backendFont = [backend createFontWithMemory:memory size:size fontSize:fontSize error:error];
    if (!backendFont) {
        [backend release];
        return nil;
    }

    RYFont *font = [[self alloc] initWithPath:nil
                                     fontSize:fontSize
                              backendInstance:backend
                                  backendFont:backendFont];
    [backend release];
    return font;
}

- (instancetype)initWithPath:(RYString *)path
                    fontSize:(float)fontSize
             backendInstance:(RYTextBackend *)backendInstance
                 backendFont:(void *)backendFont {
    self = [super init];
    if (self) {
        _path = [path retain];
        _fontSize = fontSize;
        _fallbackFonts = [[RYArray alloc] init];
        _backendInstance = [backendInstance retain];
        _backendFont = backendFont;
    }
    return self;
}

- (BOOL)addFallbackFont:(RYFont *)fallback {
    return [self addFallbackFont:fallback error:NULL];
}

- (BOOL)addFallbackFont:(RYFont *)fallback error:(RYError **)error {
    if (!fallback) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGeneralInvalidArgument
                                           message:"Fallback font is nil"];
        }
        return NO;
    }

    if (![_backendInstance addFallbackFont:fallback.backendFont toFont:_backendFont error:error]) {
        return NO;
    }

    [_fallbackFonts addObject:fallback];
    return YES;
}

- (BOOL)removeFallbackFont:(RYFont *)fallback {
    return [self removeFallbackFont:fallback error:NULL];
}

- (BOOL)removeFallbackFont:(RYFont *)fallback error:(RYError **)error {
    if (!fallback) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGeneralInvalidArgument
                                           message:"Fallback font is nil"];
        }
        return NO;
    }

    if (![_backendInstance removeFallbackFont:fallback.backendFont fromFont:_backendFont error:error]) {
        return NO;
    }

    [_fallbackFonts removeObject:fallback];
    return YES;
}

- (void)clearFallbackFonts {
    [_backendInstance clearFallbackFonts:_backendFont];
    [_fallbackFonts removeAllObjects];
}

- (void *)renderLine:(RYString *)text
               color:(RYColor)color
            outWidth:(int *)outWidth
           outHeight:(int *)outHeight
               error:(RYError **)error {
    return [_backendInstance renderLine:text
                                   font:_backendFont
                                  color:color
                               outWidth:outWidth
                              outHeight:outHeight
                                  error:error];
}

- (RYImage *)renderLineToImage:(RYString *)text
                         color:(RYColor)color
                         error:(RYError **)error {
    int w = 0;
    int h = 0;
    void *pixels = [self renderLine:text color:color outWidth:&w outHeight:&h error:error];
    if (!pixels) {
        return nil;
    }

    RYImage *image = [[RYImage alloc] initWithPixels:pixels
                                               width:w
                                              height:h
                                            channels:4
                                        freeWhenDone:YES
                                     backendInstance:nil];
    return image;
}

- (void)freePixels:(void *)pixels {
    [_backendInstance freePixels:pixels];
}

- (void)dealloc {
    if (_backendFont) {
        [_backendInstance destroyFont:_backendFont];
        _backendFont = NULL;
    }
    [_fallbackFonts release];
    [_path release];
    [_backendInstance release];
    [super dealloc];
}

@end
