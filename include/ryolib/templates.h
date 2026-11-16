#pragma once

#include <stdint.h>
#include <stdio.h>

namespace ryo {
    using HANDLE = uint32_t;
    using INDEX = uint16_t;
    using GENID = uint16_t;

    template <typename Tag>
    struct Handle {
        HANDLE raw = 0;

        constexpr bool is_null() const { return raw == 0; }
        constexpr INDEX index() const { return static_cast<INDEX>((raw >> 16) & 0xFFFF); }
        constexpr GENID gen() const { return static_cast<GENID>(raw & 0xFFFF); }
        static Handle pack(INDEX idx, GENID gen) {
            return { static_cast<HANDLE>((static_cast<uint32_t>(idx) << 16) | static_cast<uint32_t>(gen)) };
        }
    };

    template <typename TInternal, uint32_t MaxCapacity>
    struct ResourcePool {
        TInternal items[MaxCapacity + 1];
        uint16_t generations[MaxCapacity + 1] = { 0 };
        INDEX next_index = 1;
        uint32_t active_count = 0;
        INDEX free_indexes[MaxCapacity];
        uint32_t free_index_count = 0;

        INDEX allocate_index() {
            INDEX idx;
            if (free_index_count > 0) {
                idx = free_indexes[--free_index_count];
            } else {
                idx = next_index++;
            }
            active_count++;
            return idx;
        }

        void free_index(INDEX idx) {
            active_count--;
            generations[idx]++;
            // if (generations[idx] == 0) {
            //     generations[idx] = 1;
            // }
            free_indexes[free_index_count++] = idx;
            items[idx] = {};
        }

        bool is_valid(INDEX idx, GENID gen) {
            return idx != 0 && generations[idx] == gen;
        }
    };
}