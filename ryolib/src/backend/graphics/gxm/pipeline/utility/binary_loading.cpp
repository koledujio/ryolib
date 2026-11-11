#include "../private.h"

#include <cstdlib>

namespace ryo {
    void* _load_binary_file(const char* path, size_t& size) {
        FILE* file = fopen(path, "rb");
        if (file == nullptr)
            return nullptr;

        fseek(file, 0, SEEK_END);
        size = static_cast<size_t>(ftell(file));
        fseek(file, 0, SEEK_SET);

        void* data = malloc(size);
        if (data == nullptr) {
            fclose(file);
            return nullptr;
        }

        if (fread(data, 1, size, file) != size) {
            free(data);
            fclose(file);
            return nullptr;
        }

        fclose(file);
        return data;
    }
}