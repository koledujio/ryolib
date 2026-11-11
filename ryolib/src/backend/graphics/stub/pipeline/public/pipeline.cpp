#include "../../private.h"

namespace ryo {
    Pipeline::Pipeline(const Renderer& renderer, const PipelineDesc& desc) {}
    Pipeline::~Pipeline() {}

    Pipeline::Pipeline(Pipeline&&) {}
    Pipeline& Pipeline::operator=(Pipeline&&) {return *this;}

    bool Pipeline::valid() const {return true;}
}