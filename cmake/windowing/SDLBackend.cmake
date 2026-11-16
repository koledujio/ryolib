function(setup_sdl_backend TARGET_NAME)
    file(GLOB_RECURSE SDL_SOURCES CONFIGURE_DEPENDS
        "src/backend/windowing/sdl/*.cpp"
    )

    target_sources(${TARGET_NAME} PRIVATE ${SDL_SOURCES})

    if(APPLE)
        file(GLOB_RECURSE SDL_OBJCXX_SOURCES CONFIGURE_DEPENDS
            "src/backend/windowing/sdl/*.mm"
        )

        target_sources(${TARGET_NAME} PRIVATE ${SDL_OBJCXX_SOURCES})
    endif()

    # Reuse an SDL3 target provided by the parent project when possible, and
    # only fetch SDL3 when there isn't one already.
    if(NOT TARGET SDL3::SDL3-static AND NOT TARGET SDL3::SDL3)
        include(FetchContent)

        set(SDL_TESTS OFF CACHE BOOL "" FORCE)
        set(SDL_TEST  OFF CACHE BOOL "" FORCE)
        set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
        set(SDL_INSTALL OFF CACHE BOOL "" FORCE)
        set(SDL_SHARED OFF CACHE BOOL "" FORCE)
        set(SDL_STATIC ON CACHE BOOL "" FORCE)

        FetchContent_Declare(
            SDL3
            GIT_REPOSITORY https://github.com/libsdl-org/SDL
            GIT_TAG release-3.4.14
        )

        FetchContent_MakeAvailable(SDL3)

        foreach(target SDL3-static SDL3-shared SDL3_test)
            if(TARGET ${target})
                target_include_directories(${target} BEFORE PRIVATE "${SDL3_SOURCE_DIR}/include")
            endif()
        endforeach()

        target_include_directories(${TARGET_NAME} BEFORE PUBLIC "${SDL3_SOURCE_DIR}/include")
    endif()

    if(TARGET SDL3::SDL3-static)
        set(RYOLIB_SDL3_TARGET SDL3::SDL3-static)
    elseif(TARGET SDL3::SDL3)
        set(RYOLIB_SDL3_TARGET SDL3::SDL3)
    else()
        message(FATAL_ERROR "[ryolib] Failed to create an SDL3 target")
    endif()

    target_link_libraries(${TARGET_NAME} PUBLIC ${RYOLIB_SDL3_TARGET})
endfunction()
