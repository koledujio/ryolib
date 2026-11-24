function(setup_sdl_mixer_backend TARGET_NAME)
    file(GLOB_RECURSE SDL_MIXER_SOURCES CONFIGURE_DEPENDS
        "src/audio/sdl/*.cpp"
    )

    target_sources(${TARGET_NAME} PRIVATE ${SDL_MIXER_SOURCES})

    include(FetchContent)

    # Fetch SDL3 if not already defined (so that SDL_mixer can find it)
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

        target_include_directories(${TARGET_NAME} BEFORE PRIVATE "${SDL3_SOURCE_DIR}/include")

        target_link_libraries(${TARGET_NAME} PUBLIC SDL3::SDL3-static)
    endif()

    # Fetch SDL_mixer (SDL3_mixer) unless the parent project already provides it.
    if(NOT TARGET SDL3_mixer::SDL3_mixer)
        set(SDLMIXER_TESTS OFF CACHE BOOL "" FORCE)
        set(SDLMIXER_EXAMPLES OFF CACHE BOOL "" FORCE)
        set(SDLMIXER_INSTALL OFF CACHE BOOL "" FORCE)
        set(SDLMIXER_VENDORED ON CACHE BOOL "" FORCE)
        set(SDLMIXER_MP3_MPG123 OFF CACHE BOOL "" FORCE)
        set(SDLMIXER_MP3_MINIMP3 ON CACHE BOOL "" FORCE)
        set(SDLMIXER_MIDI OFF CACHE BOOL "" FORCE)
        if(IOS)
            # WavPack's vendored build uses GNU linker flags unavailable on iOS.
            set(SDLMIXER_WAVPACK OFF CACHE BOOL "" FORCE)
        endif()

        FetchContent_Declare(
            SDL3_mixer
            GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
            GIT_TAG release-3.2.4
        )

        FetchContent_MakeAvailable(SDL3_mixer)
    endif()

    target_link_libraries(${TARGET_NAME} PUBLIC SDL3_mixer::SDL3_mixer)
endfunction()