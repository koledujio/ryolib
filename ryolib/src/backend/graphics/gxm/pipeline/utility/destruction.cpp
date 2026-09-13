#include "../private.h"

#include <cstdlib>

namespace ryo {
    void _destroy_pipeline(InternalPipeline* intern) {
        if (!intern)
            return;

        if (intern->vertexProgram != nullptr)
            sceGxmShaderPatcherReleaseVertexProgram(intern->shaderPatcher, intern->vertexProgram);

        if (intern->fragmentProgram != nullptr)
            sceGxmShaderPatcherReleaseFragmentProgram(intern->shaderPatcher, intern->fragmentProgram);

        if (intern->vertexProgramId != 0)
            sceGxmShaderPatcherUnregisterProgram(intern->shaderPatcher, intern->vertexProgramId);

        if (intern->fragmentProgramId != 0)
            sceGxmShaderPatcherUnregisterProgram(intern->shaderPatcher, intern->fragmentProgramId);

        if (intern->vertexFileData != nullptr)
            free(intern->vertexFileData);

        if (intern->fragmentFileData != nullptr)
            free(intern->fragmentFileData);

        if (intern->vertexParameters != nullptr)
            delete[] intern->vertexParameters;

        if (intern->fragmentParameters != nullptr)
            delete[] intern->fragmentParameters;

        delete intern;
    }
}