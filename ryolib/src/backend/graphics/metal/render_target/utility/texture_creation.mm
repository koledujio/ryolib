#include "../private.h"

namespace ryo {
    id<MTLTexture> _render_target_create_color_texture(id<MTLDevice> device, int width, int height) {
        MTLTextureDescriptor* colorTextureDesc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatBGRA8Unorm width:width height:height mipmapped:NO];
        colorTextureDesc.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;
        colorTextureDesc.storageMode = MTLStorageModePrivate;
        return [device newTextureWithDescriptor:colorTextureDesc];
    }

    id<MTLTexture> _render_target_create_depth_texture(id<MTLDevice> device, int width, int height) {
        MTLTextureDescriptor* depthTextureDesc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float_Stencil8 width:width height:height mipmapped:NO];
        depthTextureDesc.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;
        depthTextureDesc.storageMode = MTLStorageModePrivate;
        return [device newTextureWithDescriptor:depthTextureDesc];
    }
}