#include "../private.h"

#include "ryolib/graphics.h"

#include <psp2/gxm.h>

#include <chrono>

namespace ryo {
    struct _DisplayBuffer {
        void* data = nullptr;
        SceUID memId = -1;
        
        SceGxmColorSurface surface{};
        SceGxmSyncObject* sync = nullptr;
    };

    struct InternalRenderer {
        SceGxmContext* context = nullptr;

        void* contextHostMem = nullptr;
        SceUID contextHostMemId = -1;
        void* vdmRingBuffer = nullptr;
        SceUID vdmRingBufferId = -1;
        void* vertexRingBuffer = nullptr;
        SceUID vertexRingBufferId = -1;
        void* fragmentRingBuffer = nullptr;
        SceUID fragmentRingBufferId = -1;
        void* fragmentUsseRingBuffer = nullptr;
        SceUID fragmentUsseRingBufferId = -1;
        unsigned int fragmentUsseRingOffset = 0;

        SceGxmShaderPatcher* shaderPatcher = nullptr;
        void* shaderPatcherBuffer = nullptr;
        SceUID shaderPatcherBufferId = -1;
        void* vertexUsse = nullptr;
        SceUID vertexUsseId = -1;
        unsigned int vertexUsseOffset = 0;
        void* fragmentUsse = nullptr;
        SceUID fragmentUsseId = -1;
        unsigned int fragmentUsseOffset = 0;

        unsigned int width = 0;
        unsigned int height = 0;
        unsigned int stride = 0;

        // Display buffers
        _DisplayBuffer displayBuffers[3]{};
        unsigned int frontBuffer = 0;
        unsigned int backBuffer = 1;
        SceGxmRenderTarget* renderTarget = nullptr;

        // Depth/Stencil
        SceGxmDepthStencilSurface depthSurface{};
        void* depthMem = nullptr;
        SceUID depthMemId = -1;

        // Shader programs
        Pipeline* defaultColorPipeline = nullptr;
        Pipeline* defaultTexturePipeline = nullptr;

        // Draw call buffers
        Buffer* quadVertexBuffer = nullptr;
        Buffer* quadIndexBuffer = nullptr;

        // Frame
        bool inFrame = false;
        bool inPass = false;

        // DeltaTime
        float deltaTime = 0.0f;
        std::chrono::steady_clock::time_point lastTime;
    };

    struct InternalRenderTarget {};

    struct InternalTexture {
        const Renderer* renderer = nullptr;

        SceGxmTexture texture;

        void* mem = nullptr;
        SceUID memId = -1;

        uint32_t width = 0;
        uint32_t height = 0;
    };

    struct InternalBuffer {
        void* mem = nullptr;
        SceUID memId = -1;

        size_t size;
        BufferType type;
    };

    struct InternalPipeline {
        SceGxmShaderPatcher* shaderPatcher = nullptr;

        void* vertexFileData = nullptr;
        void* fragmentFileData = nullptr;

        SceGxmShaderPatcherId vertexProgramId = 0;
        SceGxmShaderPatcherId fragmentProgramId = 0;

        SceGxmVertexProgram* vertexProgram = nullptr;
        SceGxmFragmentProgram* fragmentProgram = nullptr;

        const SceGxmProgramParameter** vertexParameters = nullptr;
        uint8_t vertexParameterCount = 0;

        const SceGxmProgramParameter** fragmentParameters = nullptr;
        uint8_t fragmentParameterCount = 0;
    };

    enum _MemoryType {
        _MEMORY_TYPE_MAIN,
        _MEMORY_TYPE_CDRAM,
        _MEMORY_TYPE_UNCACHE
    };

    enum _USSEMemoryType {
        _USSE_MEMORY_TYPE_VERTEX,
        _USSE_MEMORY_TYPE_FRAGMENT
    };

    /** 
     * @brief Aligns up a given size to 4 KiB (0x1000)
     *
     * This function aligns a given size_t size to 4 KiB pages,
     * allowing the size to be safely used for MAIN ram allocation.
     * 
     * @param[in] size The raw unaligned size
     * 
     * @return The input size, aligned up to 4 KiB
     */
    size_t _memory_align_main(size_t size);

    /** 
     * @brief Aligns up a given size to 256 KiB (0x40000)
     *
     * This function aligns a given size_t size to 256 KiB pages,
     * allowing the size to be safely used for CDRAM allocation.
     * 
     * @param[in] size The raw unaligned size
     *  
     * @return The input size, aligned up to 256 KiB
     */
    size_t _memory_align_cdram(size_t size);

    /**
     * @brief Allocates a kernel memory block of a specified size and type
     * 
     * This function allocates a kernel memory block with a specified
     * name label and memory type. The size input MUST be aligned, otherwise
     * this might fail or even crash the system.
     * 
     * @param[in] name   Name label
     * @param[in] size   Aligned size
     * @param[in] type   Memory type
     * @param[out] mem   Reference to a void pointer for the new memory block's buffer
     * @param[out] memId Reference to an SceUID for the new memory block's id
     * 
     * @pre The 'size' parameter must meet strict alignment requirements depending on the memory type:
     *       - For '_MEMORY_TYPE_MAIN' and '_MEMORY_TYPE_UNCACHE' : Must be pre-aligned to a 4 KiB (0x1000) page boundary.
     *       - For '_MEMORY_TYPE_CDRAM': Must be pre-aligned to a 256 KiB (0x40000) page boundary.
     * 
     * @return True on success, false on fail. The memId value will be modified despite failing.
     *
     * @warning Passing an unaligned size to this function might result in a system crash.
     */
    bool _memory_alloc(const char* name, size_t size, _MemoryType type, void*& mem, SceUID& memId);

    /**
     * @brief Allocates a kernel memory block and maps it as read and write
     * 
     * This function calls _memory_alloc to allocate a memory block,
     * then it maps the memory for use with the GPU for reading and writing.
     * Size is automatically aligned based on memory type as specified below:
     * 
     * - MAIN and UNCACHED -> Aligned using _memory_align_main
     * - CDRAM             -> Aligned using _memory_align_cdram
     * 
     * @param[in] name   Name label
     * @param[in] size   Raw unaligned size
     * @param[in] type   Memory type
     * @param[out] mem   Reference to a void pointer for the new memory block's buffer
     * @param[out] memId Reference to an SceUID for the new memory block's id
     * 
     * @return True on success, false on fail. The memId value will be modified
     *         despite failing, the mem value might also be modified despite failing.
     */
    bool _memory_alloc_gpu_rw(const char* name, size_t size, _MemoryType type, void*& mem, SceUID& memId);

    /**
     * @brief Allocates a kernel memory block and maps it as read-only
     * 
     * This function calls _memory_alloc to allocate a memory block,
     * then it maps the memory for use with the GPU for reading.
     * Size is automatically aligned based on memory type as specified below:
     * 
     * - MAIN and UNCACHED -> Aligned using _memory_align_main
     * - CDRAM             -> Aligned using _memory_align_cdram
     * 
     * @param[in] name   Name label
     * @param[in] size   Raw unaligned size
     * @param[in] type   Memory type
     * @param[out] mem   Reference to a void pointer for the new memory block's buffer
     * @param[out] memId Reference to an SceUID for the new memory block's id
     * 
     * @return True on success, false on fail. The memId value will be modified
     *         despite failing, the mem value might also be modified despite failing.
     */
    bool _memory_alloc_gpu_r(const char* name, size_t size, _MemoryType type, void*& mem, SceUID& memId);

    /**
     * @brief Allocates a kernel USSE memory block and maps it
     * 
     * This function calls _memory_alloc to allocate a memory block,
     * then it maps the memory for use as the USSE fragment or vertex memory.
     * Size is automatically aligned in this function.
     * 
     * @param[in] name    Name label
     * @param[in] size    Raw unaligned size
     * @param[in] type    USSE Memory type
     * @param[out] mem    Reference to a void pointer for the new memory block's buffer
     * @param[out] memId  Reference to an SceUID for the new memory block's id
     * @param[out] offset Reference to an unsigned int for the new USSE memory block's offset
     * 
     * @return True on success, false on fail
     */
    bool _memory_alloc_usse(const char* name, size_t size, _USSEMemoryType type, void*& mem, SceUID& memId, unsigned int& offset);

    /**
     * @brief Frees a specified memory block
     * 
     * This function frees the memory block located at provided id.
     * If the id is less than 0, no memory block gets freed.
     * 
     * @param[in] memId Id of the memory block
     */
    void _memory_free(SceUID memId);

    /**
     * @brief Unmaps gpu memory and frees a specified memory block
     *
     * This function unmaps gpu memory from the provided pointer and then
     * frees the memory block at the specified id.
     * If the memory pointer is null, the memory doesn't get unmapped.
     * If the id is less than 0, the memory block doesn't get freed.
     *
     * @param[in] mem   Pointer to the memory
     * @param[in] memId Id of the memory block
     */
    void _memory_free_gpu(void* mem, SceUID memId);

    /**
     * @brief Unmaps USSE memory and frees a specified memory block
     *
     * This functon unmaps USSE memory of specified type at the provided pointer
     * and frees the memory block at the specified id.
     * If the memory pointer is null, the memory doesn't get unmapped.
     * If the id is less than 0, the memory block doesn't get freed.
     *
     * @param[in] type  Type of USSE memory
     * @param[in] mem   Pointer to the memory
     * @param[in] memId Id of the memory block
     */
    void _memory_free_usse(_USSEMemoryType type, void* mem, SceUID memId);

    // Implementations for SceGxmShaderPatcherParams callbacks, used in Renderer.cpp

    void* _memory_patcher_alloc(void* userdata, SceSize size);
    void _memory_patcher_free(void* userdata, void* mem);

    void* _memory_patcher_buffer_alloc(void* userdata, SceSize size);
    void _memory_patcher_buffer_free(void* userdata, void* mem);

    void* _memory_patcher_vertex_usse_alloc(void* userdata, SceSize size, unsigned int* usseOffset);
    void _memory_patcher_vertex_usse_free(void* userdata, void* mem);

    void* _memory_patcher_fragment_usse_alloc(void* userdata, SceSize size, unsigned int* usseOffset);
    void _memory_patcher_fragment_usse_free(void* userdata, void* mem);
}