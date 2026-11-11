#import "ryolib/image/RYImage.h"
#import "ryolib/graphics.h"

extern "C" void _ry_image_texture_category_anchor(void) {}

@implementation RYImage (Texture)

- (RYTexture *)createTextureWithRenderer:(RYRenderer *)renderer {
    return [self createTextureWithRenderer:renderer error:NULL];
}

- (RYTexture *)createTextureWithRenderer:(RYRenderer *)renderer error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (!_pixels || _width <= 0 || _height <= 0) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeImageTextureCreationFailed
                                           message:"Cannot create texture: image has no valid pixel data"];
        }
        return nil;
    }

    RYTexture *texture = [RYTexture newTextureWithRenderer:renderer pixels:_pixels width:(unsigned int)_width height:(unsigned int)_height error:error];
    if (!texture) {
        if (error && !*error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeImageTextureCreationFailed
                                           message:"Failed to initialize GPU texture"];
        }
        return nil;
    }

    return texture;
}

@end
