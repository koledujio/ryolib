#include "../private.h"

namespace ryo {
    void _blend_info_gxm_get(BlendMode mode, SceGxmBlendInfo& outInfo) {
        outInfo.colorMask = SCE_GXM_COLOR_MASK_ALL;

        switch (mode) {
            case BLEND_MODE_OPAQUE:
                outInfo.colorFunc = SCE_GXM_BLEND_FUNC_NONE;
                outInfo.alphaFunc = SCE_GXM_BLEND_FUNC_NONE;
                outInfo.colorSrc = SCE_GXM_BLEND_FACTOR_ONE;
                outInfo.colorDst = SCE_GXM_BLEND_FACTOR_ZERO;
                outInfo.alphaSrc = SCE_GXM_BLEND_FACTOR_ONE;
                outInfo.alphaDst = SCE_GXM_BLEND_FACTOR_ZERO;
                break;
            
            case BLEND_MODE_ALPHA:
                outInfo.colorFunc = SCE_GXM_BLEND_FUNC_ADD;
                outInfo.alphaFunc = SCE_GXM_BLEND_FUNC_ADD;
                outInfo.colorSrc = SCE_GXM_BLEND_FACTOR_SRC_ALPHA;
                outInfo.colorDst = SCE_GXM_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
                outInfo.alphaSrc = SCE_GXM_BLEND_FACTOR_ONE;
                outInfo.alphaDst = SCE_GXM_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
                break;

            case BLEND_MODE_ADDITIVE:
                outInfo.colorFunc = SCE_GXM_BLEND_FUNC_ADD;
                outInfo.alphaFunc = SCE_GXM_BLEND_FUNC_ADD;
                outInfo.colorSrc = SCE_GXM_BLEND_FACTOR_SRC_ALPHA;
                outInfo.colorDst = SCE_GXM_BLEND_FACTOR_ONE;
                outInfo.alphaSrc = SCE_GXM_BLEND_FACTOR_ONE;
                outInfo.alphaDst = SCE_GXM_BLEND_FACTOR_ONE;
                break;
        }
    }
}