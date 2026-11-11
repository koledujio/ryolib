#include "../private.h"

#include <cstdio>
#include <cstdlib>
#include <cstddef>

namespace ryo {
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
        if (vertexFileData == nullptr) {
            error_set("Failed to load vertex shader binary");
            return;
        }

        size_t fragmentFileSize;
        void* fragmentFileData = _load_binary_file(fragmentPath, fragmentFileSize);
        if (fragmentFileData == nullptr) {
            free(vertexFileData);
            error_set("Failed to load fragment shader binary");
            return;
        }

        SceGxmProgram* vertexProgramData = static_cast<SceGxmProgram*>(vertexFileData);
        SceGxmProgram* fragmentProgramData = static_cast<SceGxmProgram*>(fragmentFileData);

        if (sceGxmProgramCheck(vertexProgramData) < 0 ||
            sceGxmProgramCheck(fragmentProgramData) < 0) {
            free(vertexFileData);
            free(fragmentFileData);
            error_set("Failed to create vertex program data or fragment program data");
            return;
        }

        SceGxmShaderPatcherId vertexProgramId;
        SceGxmShaderPatcherId fragmentProgramId;

        if (sceGxmShaderPatcherRegisterProgram(renderer.internal->shaderPatcher, vertexProgramData, &vertexProgramId) < 0) {
            free(vertexFileData);
            free(fragmentFileData);
            error_set("Failed to register vertex program data");
            return;
        }

        if (sceGxmShaderPatcherRegisterProgram(renderer.internal->shaderPatcher, fragmentProgramData, &fragmentProgramId) < 0) {
            sceGxmShaderPatcherUnregisterProgram(renderer.internal->shaderPatcher, vertexProgramId);
            free(vertexFileData);
            free(fragmentFileData);
            error_set("Failed to register fragment program data");
            return;
        }

        uint32_t registerCount = 0;
        uint32_t* registers = nullptr;
        uint8_t vertexParamCount = 0;
        const SceGxmProgramParameter** vertexParams = nullptr;
        uint8_t fragmentParamCount = 0;
        const SceGxmProgramParameter** fragmentParams = nullptr;
        if (!_get_vertex_layout_resource_indexes(vertexProgramData, fragmentProgramData, desc.vertexLayout, registers, registerCount, vertexParams, vertexParamCount, fragmentParams, fragmentParamCount)) {
            sceGxmShaderPatcherUnregisterProgram(renderer.internal->shaderPatcher, vertexProgramId);
            sceGxmShaderPatcherUnregisterProgram(renderer.internal->shaderPatcher, fragmentProgramId);
            free(vertexFileData);
            free(fragmentFileData);
            error_set("Failed to get vertex layout resource indexes");
            return;
        }

        uint32_t attributeCount = 0;
        SceGxmVertexAttribute* attributes = nullptr;
        size_t attributeSize = 0;
        if (!_get_vertex_layout_attributes(desc.vertexLayout, registers, registerCount, attributes, attributeSize, attributeCount)) {
            delete[] registers;
            sceGxmShaderPatcherUnregisterProgram(renderer.internal->shaderPatcher, vertexProgramId);
            sceGxmShaderPatcherUnregisterProgram(renderer.internal->shaderPatcher, fragmentProgramId);
            free(vertexFileData);
            free(fragmentFileData);
            error_set("Failed to register vertex layout attributes");
            return;
        }

        SceGxmVertexStream stream = {};
        stream.stride = attributeSize;
        stream.indexSource = SCE_GXM_INDEX_SOURCE_INDEX_32BIT;

        SceGxmVertexProgram* vertexProgram = nullptr;
        int vertexCreateResult = sceGxmShaderPatcherCreateVertexProgram(
            renderer.internal->shaderPatcher,
            vertexProgramId,
            attributes,
            attributeCount,
            &stream,
            1,
            &vertexProgram
        );
        if (vertexCreateResult < 0) {
            delete[] registers;
            delete[] attributes;
            sceGxmShaderPatcherUnregisterProgram(renderer.internal->shaderPatcher, vertexProgramId);
            sceGxmShaderPatcherUnregisterProgram(renderer.internal->shaderPatcher, fragmentProgramId);
            free(vertexFileData);
            free(fragmentFileData);
            error_set("Failed to create vertex program");
            return;
        }

        SceGxmBlendInfo blendInfo = {};
        _blend_info_gxm_get(desc.blendMode, blendInfo);
        const SceGxmBlendInfo* blendInfoPtr = (desc.blendMode != BLEND_MODE_OPAQUE) ? &blendInfo : nullptr;

        SceGxmFragmentProgram* fragmentProgram = nullptr;
        int fragmentCreateResult = sceGxmShaderPatcherCreateFragmentProgram(
            renderer.internal->shaderPatcher,
            fragmentProgramId,
            SCE_GXM_OUTPUT_REGISTER_FORMAT_UCHAR4,
            SCE_GXM_MULTISAMPLE_NONE,
            blendInfoPtr,
            vertexProgramData,
            &fragmentProgram
        );
        if (fragmentCreateResult < 0) {
            // delete vertexprogram
            sceGxmShaderPatcherReleaseVertexProgram(renderer.internal->shaderPatcher, vertexProgram);
            delete[] registers;
            delete[] attributes;
            sceGxmShaderPatcherUnregisterProgram(renderer.internal->shaderPatcher, vertexProgramId);
            sceGxmShaderPatcherUnregisterProgram(renderer.internal->shaderPatcher, fragmentProgramId);
            free(vertexFileData);
            free(fragmentFileData);
            error_set("Failed to create fragment program");
            return;
        }

        delete[] registers;
        delete[] attributes;

        InternalPipeline* intern = new InternalPipeline;
        intern->shaderPatcher = renderer.internal->shaderPatcher;
        intern->vertexFileData = vertexFileData;
        intern->fragmentFileData = fragmentFileData;
        intern->vertexProgramId = vertexProgramId;
        intern->fragmentProgramId = fragmentProgramId;
        intern->vertexProgram = vertexProgram;
        intern->fragmentProgram = fragmentProgram;
        intern->vertexParameters = vertexParams;
        intern->vertexParameterCount = vertexParamCount;
        intern->fragmentParameters = fragmentParams;
        intern->fragmentParameterCount = fragmentParamCount;

        internal = intern;
    }

    Pipeline::~Pipeline() {
        _destroy_pipeline(internal);
    }

    Pipeline::Pipeline(Pipeline&& other) : internal(other.internal) {
        other.internal = nullptr;
    }

    Pipeline& Pipeline::operator=(Pipeline&& other) {
        if (this == &other)
            return *this;

        _destroy_pipeline(internal);

        internal = other.internal;
        other.internal = nullptr;

        return *this;
    }

    bool Pipeline::valid() const {
        return (internal != nullptr &&

                internal->shaderPatcher != nullptr &&

                internal->vertexFileData != nullptr &&
                internal->fragmentFileData != nullptr &&

                internal->vertexProgramId != 0 &&
                internal->fragmentProgramId != 0 &&

                internal->vertexProgram != nullptr &&
                internal->fragmentProgram != nullptr);
    }
}