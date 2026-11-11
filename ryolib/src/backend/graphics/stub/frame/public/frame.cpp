#include "../../private.h"

namespace ryo {
    void Renderer::frame_begin() {}
    bool Renderer::frame_end() {return true;}

    float Renderer::frame_time_get() const {return 0.0f;}
}