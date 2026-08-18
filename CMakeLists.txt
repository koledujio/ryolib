cmake_minimum_required(VERSION 3.20)
project(GfxLibraryTest C CXX)

if(APPLE)
    enable_language(OBJC OBJCXX)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(Dependencies)
include(CopyAssets)

set(RYO_SAFETY ON CACHE BOOL "" FORCE)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(RYO_DEBUG ON CACHE BOOL "" FORCE)
else()
    set(RYO_DEBUG OFF cACHE BOOL "" FORCE)
endif()


add_subdirectory(ryolib)

add_executable(TestApp src/main.cpp)

target_link_libraries(TestApp PRIVATE ryolib glfw)

if(WIN32)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set_property(TARGET TestApp PROPERTY WIN32_EXECUTABLE FALSE)
    else()
        set_property(TARGET TestApp PROPERTY WIN32_EXECUTABLE TRUE)
    endif()

    target_link_options(TestApp PRIVATE -static -static-libgcc -static-libstdc++)
endif()

target_copy_assets(TestApp)