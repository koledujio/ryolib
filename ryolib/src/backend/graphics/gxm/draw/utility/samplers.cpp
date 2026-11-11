#include "../private.h"

namespace ryo {
    void _draw_apply_sampler_settings(SceGxmTexture* texture, SamplerType samplerType) {
        RYO_ENSURE_VOID(texture != nullptr, "Null texture pointer");

        switch (samplerType) {
            case SAMPLER_TYPE_NEAREST:
                sceGxmTextureSetMinFilter(texture, SCE_GXM_TEXTURE_FILTER_POINT);
                sceGxmTextureSetMagFilter(texture, SCE_GXM_TEXTURE_FILTER_POINT);
                break;

            case SAMPLER_TYPE_LINEAR:
                sceGxmTextureSetMinFilter(texture, SCE_GXM_TEXTURE_FILTER_LINEAR);
                sceGxmTextureSetMagFilter(texture, SCE_GXM_TEXTURE_FILTER_LINEAR);
                break;
            
            case SAMPLER_TYPE_MIN_NEAREST_MAG_LINEAR:
                sceGxmTextureSetMinFilter(texture, SCE_GXM_TEXTURE_FILTER_POINT);
                sceGxmTextureSetMagFilter(texture, SCE_GXM_TEXTURE_FILTER_LINEAR);
                break;
            
            case SAMPLER_TYPE_MIN_LINEAR_MAG_NEAREST:
                sceGxmTextureSetMinFilter(texture, SCE_GXM_TEXTURE_FILTER_LINEAR);
                sceGxmTextureSetMagFilter(texture, SCE_GXM_TEXTURE_FILTER_POINT);
                break;
        }
    }
}