#pragma once

#include "../private.h"

void _render_target_release_views(RYInternalRenderTarget& renderTarget);
bool _render_target_create_textures(ID3D11Device* device, int width, int height, RYInternalRenderTarget& result, void** outColorTexture, void** outDepthTexture);
