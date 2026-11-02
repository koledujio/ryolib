#import "ryolib/image/RYImage.h"
#include "ryolib/graphics.h"

extern "C" void _ry_image_texture_category_anchor(void) {}

@implementation RYImage (Texture)

- (ryo::Texture *)createTextureWithRenderer:(const ryo::Renderer &)renderer {
    return [self createTextureWithRenderer:renderer error:NULL];
}

- (ryo::Texture *)createTextureWithRenderer:(const ryo::Renderer &)renderer error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (!_pixels || _width <= 0 || _height <= 0) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeImageTextureCreationFailed
                                           message:"Cannot create texture: image has no valid pixel data"];
        }
        return nullptr;
    }

    ryo::Texture *texture = new ryo::Texture(renderer, _pixels, _width, _height);
    if (!texture || !texture->valid()) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeImageTextureCreationFailed
                                           message:"Failed to initialize GPU texture"];
        }
        delete texture;
        return nullptr;
    }

    return texture;
}

@end
