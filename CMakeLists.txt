cmake_minimum_required(VERSION 3.20)
project(GfxLibraryTest C CXX)

if(APPLE)
    enable_language(OBJC OBJCXX)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(VITA_APP_NAME "Ryolib UI Test")
set(VITA_TITLEID "KOLE00001")
set(VITA_VERSION "01.00")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(CopyAssets)

set(RYO_SAFETY ON CACHE BOOL "" FORCE)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(RYO_DEBUG ON CACHE BOOL "" FORCE)
else()
    set(RYO_DEBUG OFF cACHE BOOL "" FORCE)
endif()

add_subdirectory(ryolib)

add_executable(TestApp src/main.cpp)

target_link_libraries(TestApp PRIVATE ryolib)

if(WIN32)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set_property(TARGET TestApp PROPERTY WIN32_EXECUTABLE FALSE)
    else()
        set_property(TARGET TestApp PROPERTY WIN32_EXECUTABLE TRUE)
    endif()

    target_link_options(TestApp PRIVATE -static -static-libgcc -static-libstdc++)
elseif(VITA)
    include("${VITASDK}/share/vita.cmake")

    vita_create_self(ui_test.self TestApp)

    vita_create_vpk(ui_test.vpk ${VITA_TITLEID} ui_test.self
        VERSION ${VITA_VERSION}
        NAME ${VITA_APP_NAME}
        FILE "assets/mikudayo.mp3" "assets/mikudayo.mp3"
    )
endif()

if(NOT VITA)
    target_copy_assets(TestApp)
endif()