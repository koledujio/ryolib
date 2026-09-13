#include "../private.h"

#include <psp2/display.h>

namespace ryo {
    void _display_callback(const void* callback_data) {
        SceDisplayFrameBuf framebuf {};
        framebuf.size = sizeof(SceDisplayFrameBuf);
        framebuf.base = *((void **)callback_data);
        framebuf.pitch = DISPLAY_STRIDE_IN_PIXELS;
        framebuf.pixelformat = SCE_DISPLAY_PIXELFORMAT_A8B8G8R8;
        framebuf.width = DISPLAY_WIDTH;
        framebuf.height = DISPLAY_HEIGHT;

        sceDisplaySetFrameBuf(&framebuf, SCE_DISPLAY_SETBUF_NEXTFRAME);

        sceDisplayWaitVblankStart();
    }
}