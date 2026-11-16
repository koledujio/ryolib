#include "../private.h"

namespace ryo {
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
    ) {
        RYO_ASSERT(vertexProgramData != nullptr, false, "Invalid vertex file data");

        switch (layout) {
            case VERTEX_LAYOUT_POSITION: {
                const SceGxmProgramParameter* positionParam = sceGxmProgramFindParameterBySemantic(vertexProgramData, SCE_GXM_PARAMETER_SEMANTIC_POSITION, 0);
                if (positionParam == nullptr) return false;
                uint32_t positionReg = sceGxmProgramParameterGetResourceIndex(positionParam);

                const SceGxmProgramParameter* mvpParam = sceGxmProgramFindParameterByName(vertexProgramData, "MVP");
                if (mvpParam == nullptr) return false;

                const SceGxmProgramParameter* colorParam = sceGxmProgramFindParameterByName(fragmentProgramData, "Color");
                if (colorParam == nullptr) {
                    colorParam = sceGxmProgramFindParameterByName(fragmentProgramData, "StencilParams");
                }
                if (colorParam == nullptr) return false;

                registers = new uint32_t[1];
                registers[0] = positionReg;
                registerCount = 1;

                vertexParameters = new const SceGxmProgramParameter*[1];
                vertexParameters[0] = mvpParam;
                vertexParameterCount = 1;

                fragmentParameters = new const SceGxmProgramParameter*[1];
                fragmentParameters[0] = colorParam;
                fragmentParameterCount = 1;

                return true;
            }

            case VERTEX_LAYOUT_POSITION_UV: {
                const SceGxmProgramParameter* positionParam = sceGxmProgramFindParameterBySemantic(vertexProgramData, SCE_GXM_PARAMETER_SEMANTIC_POSITION, 0);
                const SceGxmProgramParameter* uvParam = sceGxmProgramFindParameterBySemantic(vertexProgramData, SCE_GXM_PARAMETER_SEMANTIC_TEXCOORD, 0);

                if (positionParam == nullptr || uvParam == nullptr)
                    return false;

                uint32_t positionReg = sceGxmProgramParameterGetResourceIndex(positionParam);
                uint32_t uvReg = sceGxmProgramParameterGetResourceIndex(uvParam);

                const SceGxmProgramParameter* mvpParam = sceGxmProgramFindParameterByName(vertexProgramData, "MVP");
                if (mvpParam == nullptr) return false;

                const SceGxmProgramParameter* colorParam = sceGxmProgramFindParameterByName(fragmentProgramData, "Color");
                if (colorParam == nullptr) return false;

                const SceGxmProgramParameter* textureParam = sceGxmProgramFindParameterByName(fragmentProgramData, "MainTexture");
                if (textureParam == nullptr) return false;

                registers = new uint32_t[2];
                registers[0] = positionReg;
                registers[1] = uvReg;
                registerCount = 2;

                vertexParameters = new const SceGxmProgramParameter*[1];
                vertexParameters[0] = mvpParam;
                vertexParameterCount = 1;

                fragmentParameters = new const SceGxmProgramParameter*[2];
                fragmentParameters[0] = colorParam;
                fragmentParameters[1] = textureParam;
                fragmentParameterCount = 2;

                return true;
            }

            default:
                return false;
        }
    }

    bool _get_vertex_layout_attributes(
        VertexLayoutType layout,
        uint32_t* registers,
        uint32_t registerCount, 
        SceGxmVertexAttribute*& attributes,
        size_t& attributeSize,
        uint32_t& attributeCount
    ) {
        switch (layout) {
            case VERTEX_LAYOUT_POSITION:
                RYO_ASSERT(registerCount == 1, false, "Incorrect vertex layout registers count");
                RYO_ASSERT(registers != nullptr, false, "Null registers pointer provided");

                attributes = new SceGxmVertexAttribute[1];
                attributes[0].streamIndex = 0;
                attributes[0].offset = 0;
                attributes[0].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
                attributes[0].componentCount = 3;
                attributes[0].regIndex = *registers;

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
}