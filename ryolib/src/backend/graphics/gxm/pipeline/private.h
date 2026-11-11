#include "../private.h"

namespace ryo {
    void _blend_info_gxm_get(BlendMode mode, SceGxmBlendInfo& outInfo);

    const char* _get_vertex_shader_filename(BuiltinShader shader);
    const char* _get_fragment_shader_filename(BuiltinShader shader);

    bool _get_vertex_layout_resource_indexes(
        const SceGxmProgram* vertexProgramData,
        const SceGxmProgram* fragmentProgramData,
        VertexLayoutType layout,
        uint32_t*& registers,
        uint32_t& registerCount,
        const SceGxmProgramParameter**& vertexParameters,
        uint8_t& vertexParameterCount,
        const SceGxmProgramParameter**& fragmentParameters,
        uint8_t& fragmentParameterCount
    );
    bool _get_vertex_layout_attributes(
        VertexLayoutType layout,
        uint32_t* registers,
        uint32_t registerCount, 
        SceGxmVertexAttribute*& attributes,
        size_t& attributeSize,
        uint32_t& attributeCount
    );

    void* _load_binary_file(const char* path, size_t& size);

    void _destroy_pipeline(InternalPipeline* intern);
}