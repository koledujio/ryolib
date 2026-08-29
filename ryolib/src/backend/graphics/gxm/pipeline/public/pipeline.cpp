#include "../../private.h"

#include <cstdio.h>
#include <cstdlib.h>

namespace ryo {
    static const char* _get_vertex_shader_filename(BuiltinShader shader) {
        switch (shader) {
            case BUILTIN_SHADER_COLOR:
                return "color_v.gxp";

            case BUILTIN_SHADER_TEXTURE:
                return nullptr;

            default:
                return nullptr;
        }
    }

    static const char* _get_fragment_shader_filename(BuiltinShader shader) {
        switch (shader) {
            case BUILTIN_SHADER_COLOR:
                return "color_f.gxp";

            case BUILTIN_SHADER_TEXTURE:
                return nullptr;

            default:
                return nullptr;
        }
    }

    static void* _load_binary_file(const char* path, size_t& size) {
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

    Pipeline::Pipeline(const Renderer& renderer, const PipelineDesc& desc) {
        RYO_ENSURE_VOID(renderer.valid(), "Invalid renderer");

        const char* vertexFilename = _get_vertex_shader_filename(desc.shader);
        const char* fragmentFilename = _get_fragment_shader_filename(desc.shader);
        RYO_ASSERT_VOID(vertexFilename != nullptr && fragmentFilename != nullptr, "Failed to get shader filename");

        char vertexPath[256];
        char fragmentPath[256];

        snprintf(vertexPath, sizeof(vertexPath), "shaders/%s", vertexFilename);
        snprintf(fragmentPath, sizeof(fragmentPath), "shaders/%s", fragmentFilename);

        size_t vertexFileSize;
        void* vertexFileData = _load_binary_file(vertexPath, vertexFileSize);
        if (vertexFileData == nullptr)
            return;

        size_t fragmentFileSize;
        void* fragmentFileData = _load_binary_file(fragmentPath, fragmentFileSize);
        if (fragmentFileData == nullptr) {
            free(vertexFileData);
            return;
        }

        SceGxmProgram* vertexProgram = static_cast<SceGxmProgram*>(vertexFileData);
        SceGxmProgram* fragmentProgram = static_cast<SceGxmProgram*>(fragmentFileData);

        if (sceGxmProgramCheck(vertexProgram) < 0 ||
            sceGxmProgramCheck(fragmentProgram) < 0) {
            free(vertexFileData);
            free(fragmentFileData);
            return;
        }

        SceGxmShaderPatcherId vertexProgramId;
        SceGxmShaderPatcherId fragmentProgramId;

        if (sceGxmShaderPatcherRegisterProgram(renderer.internal->shaderPatcher, vertexProgram, &vertexProgramId) < 0) {
            free(vertexFileData);
            free(fragmentFileData);
            return;
        }

        if (sceGxmShaderPatcherRegisterProgram(renderer.internal->shaderPatcher, fragmentProgram, &fragmentProgramId) < 0) {
            sceGxmShaderPatcherUnregisterProgram(renderer.internal->shaderPatcher, vertexProgramId);
            free(vertexFileData);
            free(fragmentFileData);
            return;
        }


    }

    Pipeline::~Pipeline() {

    }

    Pipeline::Pipeline(Pipeline&& other) : internal(other.internal) {
        other.internal = nullptr;
    }

    Pipeline& Pipeline::operator=(Pipeline&& other) {
        if (this == &other)
            return *this;

        if (internal) {
            delete internal;
        }

        internal = other.internal;
        other.internal = nullptr;

        return *this;
    }

    bool Pipeline::valid() const {return true;}
}