#include "../../private.h"

#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include <cstdio>

namespace ryo {
    static const char* _get_vertex_shader_filename(BuiltinShader shader) {
        switch (shader) {
            case BUILTIN_SHADER_COLOR:
                return "color_v.gxp";

            case BUILTIN_SHADER_TEXTURE:
                return "texture_v.gxp";

            default:
                return nullptr;
        }
    }

    static const char* _get_fragment_shader_filename(BuiltinShader shader) {
        switch (shader) {
            case BUILTIN_SHADER_COLOR:
                return "color_f.gxp";

            case BUILTIN_SHADER_TEXTURE:
                return "texture_f.gxp";

            default:
                return nullptr;
        }
    }

    static bool _get_vertex_layout_resource_indexes(const SceGxmProgram* vertexProgramData, VertexLayoutType layout, uint32_t*& registers, uint32_t& registerCount, FILE* log) {
        RYO_ASSERT(vertexProgramData != nullptr, false, "Invalid vertex file data");

        switch (layout) {
            case VERTEX_LAYOUT_POSITION: {
                fprintf(log, "Before sceGxmProgramFindParameterByName\n");
                fflush(log);

                const SceGxmProgramParameter* positionParam = sceGxmProgramFindParameterBySemantic(vertexProgramData, SCE_GXM_PARAMETER_SEMANTIC_POSITION, 0);

                fprintf(log, "After sceGxmProgramFindParameterByName\n");
                fflush(log);

                // if (positionParam == nullptr) { // crash might be here too
                //     registers = nullptr;
                //     registerCount = 0;
                //     return false;
                // }

                if (positionParam == nullptr)
                    return false;

                fprintf(log, "Before sceGxmProgramParameterGetResourceIndex\n");
                fflush(log);
                
                uint32_t positionReg = sceGxmProgramParameterGetResourceIndex(positionParam); // crash is definitely here

                fprintf(log, "After sceGxmProgramParameterGetResourceIndex\n");
                fflush(log);

                registers = new uint32_t;
                *registers = positionReg;
                registerCount = 1;

                return true;
            }

            case VERTEX_LAYOUT_POSITION_UV: {
                const SceGxmProgramParameter* positionParam = sceGxmProgramFindParameterBySemantic(vertexProgramData, SCE_GXM_PARAMETER_SEMANTIC_POSITION, 0);
                const SceGxmProgramParameter* uvParam = sceGxmProgramFindParameterBySemantic(vertexProgramData, SCE_GXM_PARAMETER_SEMANTIC_TEXCOORD, 0);

                if (positionParam == nullptr || uvParam == nullptr)
                    return false;

                uint32_t positionReg = sceGxmProgramParameterGetResourceIndex(positionParam);
                uint32_t uvReg = sceGxmProgramParameterGetResourceIndex(uvParam);

                registers = new uint32_t[2];
                registers[0] = positionReg;
                registers[1] = uvReg;
                registerCount = 2;

                return true;
            }

            default:
                return false;
        }
    }

    static bool _get_vertex_layout_attributes(VertexLayoutType layout, uint32_t* registers, uint32_t registerCount, SceGxmVertexAttribute*& attributes, size_t& attributeSize, uint32_t& attributeCount) {
        switch (layout) {
            case VERTEX_LAYOUT_POSITION:
                RYO_ASSERT(registerCount == 1, false, "Incorrect vertex layout registers count");
                RYO_ASSERT(registers != nullptr, false, "Null registers pointer provided");

                attributes = new SceGxmVertexAttribute;
                attributes->streamIndex = 0;
                attributes->offset = 0;
                attributes->format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
                attributes->componentCount = 3;
                attributes->regIndex = *registers;

                attributeSize = sizeof(PositionVertex);
                attributeCount = 1;

                return true;

            case VERTEX_LAYOUT_POSITION_UV:
                RYO_ASSERT(registerCount == 2, false, "Incorrect vertex layout registers count");
                RYO_ASSERT(registers != nullptr, false, "Null registers pointer provided");

                attributes = new SceGxmVertexAttribute[2];

                attributes[0].streamIndex = 0;
                attributes[0].offset = offsetof(PositionUVVertex, position);
                attributes[0].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
                attributes[0].componentCount = 3;
                attributes[0].regIndex = registers[0];

                attributes[1].streamIndex = 0;
                attributes[1].offset = offsetof(PositionUVVertex, uv);
                attributes[1].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
                attributes[1].componentCount = 2;
                attributes[1].regIndex = registers[1];

                attributeSize = sizeof(PositionUVVertex);
                attributeCount = 2;

                return true;

            default:
                return false;
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
        FILE* log = fopen("ux0:data/ryolib-pipeline.log", "a");

        RYO_ENSURE_VOID(renderer.valid(), "Invalid renderer");

        const char* vertexFilename = _get_vertex_shader_filename(desc.shader);
        const char* fragmentFilename = _get_fragment_shader_filename(desc.shader);
        RYO_ASSERT_VOID(vertexFilename != nullptr && fragmentFilename != nullptr, "Failed to get shader filename");

        fprintf(log, "Shader filenames found\n");
        fflush(log);

        char vertexPath[256];
        char fragmentPath[256];

        snprintf(vertexPath, sizeof(vertexPath), "shaders/%s", vertexFilename);
        snprintf(fragmentPath, sizeof(fragmentPath), "shaders/%s", fragmentFilename);

        fprintf(log, "Shader path completed\n");
        fflush(log);

        size_t vertexFileSize;
        void* vertexFileData = _load_binary_file(vertexPath, vertexFileSize);
        if (vertexFileData == nullptr) {
            error_set("Failed to load vertex shader binary");
            return;
        }

        fprintf(log, "Vertex file data loaded\n");
        fflush(log);

        size_t fragmentFileSize;
        void* fragmentFileData = _load_binary_file(fragmentPath, fragmentFileSize);
        if (fragmentFileData == nullptr) {
            free(vertexFileData);
            error_set("Failed to load fragment shader binary");
            return;
        }

        fprintf(log, "Fragment file data loaded\n");
        fflush(log);

        SceGxmProgram* vertexProgramData = static_cast<SceGxmProgram*>(vertexFileData);
        SceGxmProgram* fragmentProgramData = static_cast<SceGxmProgram*>(fragmentFileData);

        if (sceGxmProgramCheck(vertexProgramData) < 0 ||
            sceGxmProgramCheck(fragmentProgramData) < 0) {
            free(vertexFileData);
            free(fragmentFileData);
            error_set("Failed to create vertex program data or fragment program data");
            return;
        }

        fprintf(log, "Vertex and fragment program data created\n");
        fflush(log);

        SceGxmShaderPatcherId vertexProgramId;
        SceGxmShaderPatcherId fragmentProgramId;

        if (sceGxmShaderPatcherRegisterProgram(renderer.internal->shaderPatcher, vertexProgramData, &vertexProgramId) < 0) {
            free(vertexFileData);
            free(fragmentFileData);
            error_set("Failed to register vertex program data");
            return;
        }

        fprintf(log, "Vertex program data registered\n");
        fflush(log);

        if (sceGxmShaderPatcherRegisterProgram(renderer.internal->shaderPatcher, fragmentProgramData, &fragmentProgramId) < 0) {
            sceGxmShaderPatcherUnregisterProgram(renderer.internal->shaderPatcher, vertexProgramId);
            free(vertexFileData);
            free(fragmentFileData);
            error_set("Failed to register fragment program data");
            return;
        }

        fprintf(log, "Fragment program data registered\n");
        fflush(log);

        uint32_t registerCount = 0;
        uint32_t* registers = nullptr;
        if (!_get_vertex_layout_resource_indexes(vertexProgramData, desc.vertexLayout, registers, registerCount, log)) {
            sceGxmShaderPatcherUnregisterProgram(renderer.internal->shaderPatcher, vertexProgramId);
            sceGxmShaderPatcherUnregisterProgram(renderer.internal->shaderPatcher, fragmentProgramId);
            free(vertexFileData);
            free(fragmentFileData);
            error_set("Failed to get vertex layout resource indexes");
            return;
        }

        fprintf(log, "Vertex layout resource indexes retrieved\n");
        fflush(log);

        uint32_t attributeCount = 0;
        SceGxmVertexAttribute* attributes = nullptr;
        size_t attributeSize = 0;
        if (!_get_vertex_layout_attributes(desc.vertexLayout, registers, registerCount, attributes, attributeSize, attributeCount)) {
            delete registers;
            sceGxmShaderPatcherUnregisterProgram(renderer.internal->shaderPatcher, vertexProgramId);
            sceGxmShaderPatcherUnregisterProgram(renderer.internal->shaderPatcher, fragmentProgramId);
            free(vertexFileData);
            free(fragmentFileData);
            error_set("Failed to register vertex layout attributes");
            return;
        }

        fprintf(log, "Vertex layout attributes retrieved\n");
        fflush(log);

        SceGxmVertexStream stream;
        // stream.streamIndex = 0;
        stream.stride = attributeSize;
        stream.indexSource = SCE_GXM_INDEX_SOURCE_INDEX_32BIT; // I use 32-bit int indices.

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
            delete registers;
            delete attributes;
            sceGxmShaderPatcherUnregisterProgram(renderer.internal->shaderPatcher, vertexProgramId);
            sceGxmShaderPatcherUnregisterProgram(renderer.internal->shaderPatcher, fragmentProgramId);
            free(vertexFileData);
            free(fragmentFileData);
            error_set("Failed to create vertex program");
            return;
        }

        fprintf(log, "Vertex program created\n");
        fflush(log);

        SceGxmFragmentProgram* fragmentProgram = nullptr;
        int fragmentCreateResult = sceGxmShaderPatcherCreateFragmentProgram(
            renderer.internal->shaderPatcher,
            fragmentProgramId,
            SCE_GXM_OUTPUT_REGISTER_FORMAT_UCHAR4,
            SCE_GXM_MULTISAMPLE_NONE,
            nullptr,
            vertexProgramData,
            &fragmentProgram
        );
        if (fragmentCreateResult < 0) {
            // delete vertexprogram
            sceGxmShaderPatcherReleaseVertexProgram(renderer.internal->shaderPatcher, vertexProgram);
            delete registers;
            delete attributes;
            sceGxmShaderPatcherUnregisterProgram(renderer.internal->shaderPatcher, vertexProgramId);
            sceGxmShaderPatcherUnregisterProgram(renderer.internal->shaderPatcher, fragmentProgramId);
            free(vertexFileData);
            free(fragmentFileData);
            error_set("Failed to create fragment program");
            return;
        }

        fprintf(log, "Fragment program created\n");
        fflush(log);

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

        internal = intern;

        fprintf(log, "Done!\n");
        fflush(log);
    }

    Pipeline::~Pipeline() {
        if (!internal)
            return;

        if (internal->vertexProgram != nullptr)
            sceGxmShaderPatcherReleaseVertexProgram(internal->shaderPatcher, internal->vertexProgram);

        if (internal->fragmentProgram != nullptr)
            sceGxmShaderPatcherReleaseFragmentProgram(internal->shaderPatcher, internal->fragmentProgram);

        if (internal->vertexProgramId != 0)
            sceGxmShaderPatcherUnregisterProgram(internal->shaderPatcher, internal->vertexProgramId);

        if (internal->fragmentProgramId != 0)
            sceGxmShaderPatcherUnregisterProgram(internal->shaderPatcher, internal->fragmentProgramId);

        if (internal->vertexFileData != nullptr)
            free(internal->vertexFileData);

        if (internal->fragmentFileData != nullptr)
            free(internal->fragmentFileData);

        delete internal;
    }

    Pipeline::Pipeline(Pipeline&& other) : internal(other.internal) {
        other.internal = nullptr;
    }

    Pipeline& Pipeline::operator=(Pipeline&& other) {
        if (this == &other)
            return *this;

        if (internal) {
            if (internal->vertexProgram != nullptr)
                sceGxmShaderPatcherReleaseVertexProgram(internal->shaderPatcher, internal->vertexProgram);

            if (internal->fragmentProgram != nullptr)
                sceGxmShaderPatcherReleaseFragmentProgram(internal->shaderPatcher, internal->fragmentProgram);

            if (internal->vertexProgramId != 0)
                sceGxmShaderPatcherUnregisterProgram(internal->shaderPatcher, internal->vertexProgramId);

            if (internal->fragmentProgramId != 0)
                sceGxmShaderPatcherUnregisterProgram(internal->shaderPatcher, internal->fragmentProgramId);

            if (internal->vertexFileData != nullptr)
                free(internal->vertexFileData);

            if (internal->fragmentFileData != nullptr)
                free(internal->fragmentFileData);

            delete internal;
        }

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