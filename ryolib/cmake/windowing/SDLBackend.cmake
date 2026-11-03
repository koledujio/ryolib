function(setup_sdl_backend TARGET_NAME)
    file(GLOB_RECURSE SDL_OBJC_SOURCES CONFIGURE_DEPENDS
        "src/backend/windowing/sdl/*.m"
    )

    target_sources(${TARGET_NAME} PRIVATE ${SDL_OBJC_SOURCES})

    file(GLOB_RECURSE SDL_OBJCXX_SOURCES CONFIGURE_DEPENDS
        "src/backend/windowing/sdl/*.mm"
    )

    target_sources(${TARGET_NAME} PRIVATE ${SDL_OBJCXX_SOURCES})

    file(GLOB_RECURSE SDL_C_SOURCES CONFIGURE_DEPENDS
        "src/backend/windowing/sdl/*.c"
    )

    target_sources(${TARGET_NAME} PRIVATE ${SDL_C_SOURCES})

    file(GLOB_RECURSE SDL_SOURCES CONFIGURE_DEPENDS
        "src/backend/windowing/sdl/*.cpp"
    )

    target_sources(${TARGET_NAME} PRIVATE ${SDL_SOURCES})

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

    target_link_libraries(${TARGET_NAME} PUBLIC SDL3::SDL3-static)
endfunction()
