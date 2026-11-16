function(setup_sdl_ttf_backend TARGET_NAME)
    file(GLOB_RECURSE SDL_TTF_SOURCES CONFIGURE_DEPENDS
        "src/backend/text/sdl_ttf/*.cpp"
    )

    target_sources(${TARGET_NAME} PRIVATE ${SDL_TTF_SOURCES})

    include(FetchContent)

    # Fetch SDL3 if not already defined (so that SDL_ttf can find it)
    if(NOT TARGET SDL3::SDL3-static AND NOT TARGET SDL3::SDL3)
        set(SDL_TESTS OFF CACHE BOOL "" FORCE)
        set(SDL_TEST  OFF CACHE BOOL "" FORCE)
        set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
        set(SDL_INSTALL OFF CACHE BOOL "" FORCE)
        set(SDL_SHARED OFF CACHE BOOL "" FORCE)
        set(SDL_STATIC ON CACHE BOOL "" FORCE)

        FetchContent_Declare(
            SDL3
            GIT_REPOSITORY https://github.com/libsdl-org/SDL
            GIT_TAG release-3.4.12
        )
        FetchContent_MakeAvailable(SDL3)

        foreach(target SDL3-static SDL3-shared SDL3_test)
            if(TARGET ${target})
                target_include_directories(${target} BEFORE PRIVATE "${SDL3_SOURCE_DIR}/include")
            endif()
        endforeach()

        if(VITA AND TARGET SDL3-static)
            # SDL ships its own stb_image copy; avoid emulated TLS on the Vita.
            target_compile_definitions(SDL3-static PRIVATE STBI_NO_THREAD_LOCALS)
        endif()

        target_include_directories(${TARGET_NAME} BEFORE PRIVATE "${SDL3_SOURCE_DIR}/include")

        target_link_libraries(${TARGET_NAME} PUBLIC SDL3::SDL3-static)
    endif()

    # Fetch SDL_ttf (SDL3_ttf) unless the parent project already provides it.
    if(NOT TARGET SDL3_ttf::SDL3_ttf)
        set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
        set(SDLTTF_SAMPLES OFF CACHE BOOL "" FORCE)
        set(SDLTTF_INSTALL OFF CACHE BOOL "" FORCE)
        set(SDLTTF_VENDORED ON CACHE BOOL "" FORCE) # Ensures it builds bundled FreeType/HarfBuzz if not found on system

        if(VITA)
            set(SDLTTF_HARFBUZZ OFF CACHE BOOL "" FORCE)
        else()
            set(SDLTTF_HARFBUZZ ON CACHE BOOL "" FORCE)
        endif()

        set(FT_DISABLE_ZLIB ON CACHE BOOL "" FORCE)
        set(FT_DISABLE_BZIP2 ON CACHE BOOL "" FORCE)
        set(FT_DISABLE_PNG ON CACHE BOOL "" FORCE)
        set(FT_DISABLE_BROTLI ON CACHE BOOL "" FORCE)
        set(FT_REQUIRE_ZLIB OFF CACHE BOOL "" FORCE)
        set(FT_REQUIRE_BZIP2 OFF CACHE BOOL "" FORCE)
        set(FT_REQUIRE_PNG OFF CACHE BOOL "" FORCE)
        set(FT_REQUIRE_BROTLI OFF CACHE BOOL "" FORCE)

        set(HB_BUILD_TESTS OFF CACHE BOOL "" FORCE)
        set(HB_BUILD_UTILS OFF CACHE BOOL "" FORCE)
        set(HB_BUILD_SUBSET OFF CACHE BOOL "" FORCE)

        FetchContent_Declare(
            SDL3_ttf
            GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
            GIT_TAG release-3.2.0
        )

        if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            add_compile_options("-Wa,-mbig-obj")
        endif()

        FetchContent_MakeAvailable(SDL3_ttf)

        if(VITA)
            # plutovg bundles a copy of stb_image that uses thread_local
            # storage. The Vita toolchain turns that into emulated TLS
            # relocations, which vita-elf-create rejects.
            if(TARGET plutovg)
                target_compile_definitions(plutovg PRIVATE STBI_NO_THREAD_LOCALS)
            endif()
            if(TARGET SDL3_ttf-static)
                target_compile_definitions(SDL3_ttf-static PRIVATE STBI_NO_THREAD_LOCALS)
            endif()
        endif()
    endif()

    target_link_libraries(${TARGET_NAME} PUBLIC SDL3_ttf::SDL3_ttf)

    if(APPLE)
        find_library(CORETEXT_FRAMEWORK CoreText REQUIRED)
        target_link_libraries(${TARGET_NAME} PUBLIC ${CORETEXT_FRAMEWORK})
    endif()
endfunction()