#include "../private.h"

#define ALIGN_UP(x, align) (((x) + ((align) - 1)) & ~((align) - 1))

namespace ryo {
    size_t _memory_align_main(size_t size) {
        return ALIGN_UP(size, 0x1000);
    }

    size_t _memory_align_cdram(size_t size) {
        return ALIGN_UP(size, 0x40000);
    }
}