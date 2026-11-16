# ryolib

A small cross-platform graphics, windowing, text, image and audio library.

The library is the repository's top-level project, there's a demo application in `examples/`.

## Project layout

```
CMakeLists.txt        library project and target definition
include/ryolib/       public headers
src/                  library implementation and built-in shaders
cmake/                platform/backend CMake modules
examples/             example application, assets and build helpers
examples/build-*.sh   convenience scripts for the example app
examples/build*/      generated build directories (gitignored)
```

## Building the example application

The example is built automatically when ryolib is the top-level project. Run
CMake from the `examples/` directory so the build output stays next to the
example:

```sh
cd examples
cmake -S .. -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

Or use one of the provided scripts:

```sh
./examples/build-mac.sh   # can only be built on macOS hosts
./examples/build-win32.sh # cross-compiles, supported on linux and macOS hosts
./examples/build-vita.sh  # cross-compiles, supported on linux and macOS hosts
./examples/build-ios.sh   # generates an Xcode project that needs to be compiled sepeerately
```

Build directories are created relative to the directory where CMake is run.
The scripts `cd` into `examples/` first, so they always create
`examples/build-*/` no matter where you invoke them. Manual commands create
whatever directory you pass to `-B`; the example above creates `examples/build/`.

The executable is written to `<build-dir>/examples/` (or
`<build-dir>/examples/<Config>/` with multi-config generators) and its assets
are copied next to it. For example: `examples/build/examples/TestApp` for the
manual build above, or `examples/build-mac/examples/TestApp` for
`build-mac.sh`.

### Note on the PlayStation Vita port

`examples/build-vita.sh` finds the Vita SDK in `$VITASDK`, `/usr/local/vitasdk`, or
`~/vitasdk`. The GXM shaders are compiled with `psp2cgc.exe`, which is a Windows
binary, so a host without Windows also needs Wine. On macOS the script uses the
`wine` binary inside `Wine Devel.app` / `Wine Staging.app` when it is not on
`PATH`. To use a specific Wine build:

```sh
cd examples
cmake -S .. -B build-vita \
    -DCMAKE_TOOLCHAIN_FILE="$VITASDK/share/vita.toolchain.cmake" \
    -DWINE_EXECUTABLE=/path/to/wine
cmake --build build-vita
```

The resulting package is `examples/build-vita/examples/ui_test.vpk`.

## Using ryolib in another project

### Using ryolib as a library

Here are the supported ways of using ryolib as a library in another prooject using cmake:

#### Git submodule / vendored copy

```cmake
add_subdirectory(third_party/ryolib)

add_executable(mygame src/main.cpp)
target_link_libraries(mygame PRIVATE ryolib::ryolib)
```

`RYOLIB_BUILD_EXAMPLES` defaults to `OFF` when ryolib is added through
`add_subdirectory()`, so the demo app is not built by your project unless you
explicitly enable it.

#### FetchContent

```cmake
include(FetchContent)

FetchContent_Declare(
    ryolib
    GIT_REPOSITORY https://github.com/your-name/ryolib.git
    GIT_TAG main
)
FetchContent_MakeAvailable(ryolib)

target_link_libraries(mygame PRIVATE ryolib::ryolib)
```

### Notes on graphics subsystem runtime files

#### Metal

The Metal backend loads `shaders.metallib` at runtime. To copy the shader
library (and optionally an asset directory) next to your executable, include
the helper module and call it on your target:

```cmake
# vendored as a submodule
include(${CMAKE_CURRENT_SOURCE_DIR}/third_party/ryolib/cmake/CopyAssets.cmake)

# or, when using FetchContent
# include(${ryolib_SOURCE_DIR}/cmake/CopyAssets.cmake)

ryolib_copy_assets(mygame
    ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets"
)
```

`ASSETS_DIR` is optional. On macOS/iOS the files are embedded in the
application bundle; on other platforms they are copied next to the executable.
The compiled Metal shader library is found through the `RYOLIB_METALLIB`
property on the `ryolib` target.

#### D3D11

The D3D11 backend has no runtime shader files. Its built-in HLSL shaders
(`color`, `texture` and `stencil`) are compiled while building `ryolib`:

- If `fxc` is found on the build machine, the shaders are compiled to bytecode
  and embedded into the library.
- Otherwise the raw HLSL source is embedded and compiled at runtime through
  `d3dcompiler`, which `ryolib` already links. This makes cross-compiling from
  macOS/Linux work without the Windows SDK, at the cost of a one-time compile
  when each pipeline is created.

Either way the shaders are part of `libryolib`, so nothing extra needs to be
copied next to the executable.

#### GXM

The GXM backend loads compiled shader binaries from disk at runtime. The
following six files are required, relative to the application's working
directory:

```
shaders/color_v.gxp
shaders/color_f.gxp
shaders/texture_v.gxp
shaders/texture_f.gxp
shaders/stencil_v.gxp
shaders/stencil_f.gxp
```

They are produced by the `ryolib_gxm_shaders` target (using `psp2cgc.exe`,
see above) and written to the `RYOLIB_GXM_SHADER_DIR` property on the `ryolib`
target. Package them into your VPK under `shaders/`; the example application
does this in `examples/CMakeLists.txt`:

```cmake
get_target_property(_ryolib_gxm_shader_dir ryolib RYOLIB_GXM_SHADER_DIR)
if(NOT _ryolib_gxm_shader_dir)
    set(_ryolib_gxm_shader_dir "${CMAKE_BINARY_DIR}/shaders")
endif()

file(GLOB_RECURSE VITA_SHADERS
    CONFIGURE_DEPENDS
    "${_ryolib_gxm_shader_dir}/*"
)

set(VITA_VPK_FILES "")
foreach(SHADER ${VITA_SHADERS})
    if(IS_DIRECTORY "${SHADER}")
        continue()
    endif()

    get_filename_component(SHADER_NAME "${SHADER}" NAME)

    list(APPEND VITA_VPK_FILES
        "${SHADER}" "shaders/${SHADER_NAME}"
    )
endforeach()

vita_create_vpk(ui_test.vpk ${VITA_TITLEID} ui_test.self
    VERSION ${VITA_VERSION}
    NAME ${VITA_APP_NAME}
    FILE ${VITA_VPK_FILES}
)
```

## CMake options

| Option | Default | Description |
| --- | --- | --- |
| `RYOLIB_BUILD_EXAMPLES` | `ON` top-level, `OFF` as a subproject | Build the example application. |
| `RYO_DEBUG` | `ON` in `Debug`, otherwise `OFF` | Compile in debug assertions that abort on invalid use. |
| `RYO_SAFETY` | `ON` | Compile in runtime safety checks that report an error and return instead of crashing. |

## Supported platforms

ryolib has first-class support for macOS and Windows with second-class support for iOS and the Playstation Vita. The project is developed on macOS and manually (for now) tested on macOS and Windows every commit. iOS and Playstation Vita platforms are tested less often, mostly  for more significant commits.

| Platform | Audio | Graphics | Image decoding | Text rasterization | Windowing |
| --- | --- | --- | --- | --- | --- |
| macOS | **Miniaudio**, SDL_Mixer | **Metal** | **stb_image** | **SDL_TTF** | **SDL** |
| iOS / iPadOS | **SDL_Mixer**, Miniaudio | **Metal** | **stb_image** | **SDL_TTF** | **SDL** |
| Windows | **Miniaudio**, SDL_Mixer | **D3D11** | **stb_image** | **SDL_TTF** | **SDL** |
| PlayStation Vita | **SDL_Mixer** | **GXM** | **stb_image** | **SDL_TTF** | **SDL** |

Linux and BSD ports are planned to be added relatively *soon™*, but are not supported yet due to no compatible implemented graphics backends.

Android and Nintendo Switch ports are also planned, but require more work before they can be implemented, specifically missing graphics backends need to be implemented and the build system requires more configuration.