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

The example is a separate CMake project in `examples/`. Configure it from that directory so the build output stays next to the example:

```sh
cd examples
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

Or use one of the provided scripts:

```sh
./examples/build-mac.sh   # can only be built on macOS hosts
./examples/build-win32.sh # cross-compiles, supported on linux and macOS hosts
./examples/build-vita.sh  # cross-compiles, supported on linux and macOS hosts
./examples/build-ios.sh   # generates an Xcode project that needs to be compiled separately
```

Build directories are created relative to the directory where CMake is run.
The scripts `cd` into `examples/` first, so they always create `examples/build-*/` no matter where you invoke them.
Manual commands create whatever directory you pass to `-B`, the example above creates `examples/build/`.

The example application is written to a `dist` folder inside the build
directory, together with the assets and runtime shader files it needs:

- `examples/build/dist/TestApp.app` for the manual build above
- `examples/build-mac/dist/TestApp.app` for `build-mac.sh`
- `examples/build-win32/dist/TestApp.exe` for `build-win32.sh`
- `examples/build-vita/dist/TestApp.vpk` for `build-vita.sh`
- `examples/build-ios/dist/TestApp.app` for `build-ios.sh`

The `dist` folder contains only the files needed to run or ship the app, so it can be copied out of the build tree as-is.
Pass `-DRYOLIB_OUTPUT_DIR=/some/output/dir` (or a relative path such as `output`, resolved against the build directory) to use a different folder.
The rest of the build tree and intermediate files stay under the build directory.

On Windows, the executable needs to be in the same directory as the `assets` directory.

The macOS, iOS and PS Vita builds embed and locate their resources in their respective bundles. (.app for macOS and iOS, .vpk for PS Vita).

### Note on the PlayStation Vita port

`examples/build-vita.sh` finds the Vita SDK in `$VITASDK`, `/usr/local/vitasdk`, or `~/vitasdk`. GXM shaders are compiled with `psp2cgc.exe`, which is a Windows binary, so a host without Windows also needs Wine.
On macOS the script uses the `wine` binary inside `Wine Devel.app` / `Wine Staging.app` when it is not on `PATH`,
on Linux only the default `wine` binary specified in the `PATH` is used unless a custom one is specified.

To use a specific Wine build specify the `-DWINE_EXECUTABLE` argument when configuring cmake, below is an example on how to do this.

```sh
cd examples
cmake -S . -B build-vita \
    -DCMAKE_TOOLCHAIN_FILE="$VITASDK/share/vita.toolchain.cmake" \
    -DWINE_EXECUTABLE=/path/to/wine
cmake --build build-vita
```

The resulting package is `examples/build-vita/dist/TestApp.vpk`, it embeds every required resource.

## Using ryolib in another project

### Using ryolib as a library

ryolib can be easily used as an library in another C/C++/Obj-C/Obj-C++ project using the CMake build system,
a fuller example of how this can be done using `add_subdirectory` is the examples app in the `examples` directory in this repo. I recommend you pin the ryolib version you are using to a specific commit or version as to not have frequent issues from ryolib updates. Below you can find a list of multiple minimal ways of including `ryolib` in another project using CMake.

#### Git submodule / vendored copy

```cmake
add_subdirectory(third_party/ryolib)

add_executable(myproject src/main.cpp)
target_link_libraries(myproject PRIVATE ryolib::ryolib)
```

#### FetchContent

```cmake
include(FetchContent)

FetchContent_Declare(
    ryolib
    GIT_REPOSITORY https://github.com/koledujio/ryolib.git
    GIT_TAG main # recommended to change this to a specific commit or release version
)
FetchContent_MakeAvailable(ryolib)

target_link_libraries(myproject PRIVATE ryolib::ryolib)
```

### Assets and application packaging

Since various platforms supported by ryolib have different packaging standards,
`ryolib` provides helper functions for packaging resources for supported platforms.
Below you can find examples on using the before mentioned helper functions in CMake.

```cmake
# vendored as a submodule
# replace "third_party/" with the directory the "ryolib" source code is in
include(${CMAKE_CURRENT_SOURCE_DIR}/third_party/ryolib/cmake/CopyAssets.cmake)

# when using FetchContent
include(${ryolib_SOURCE_DIR}/cmake/CopyAssets.cmake)
```

Declare assets with `ryolib_add_assets()` and let `ryolib_package_app()` stage everything the application needs at runtime:

```cmake
ryolib_add_assets(myproject
    ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets"
)

ryolib_package_app(myproject)
```

`ryolib_add_assets()` also accepts individual files and a destination directory:

```cmake
ryolib_add_assets(myproject
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/config.json"
    DESTINATION "data"
)
```

`ryolib_package_app()` writes the final executable (or application bundle) to `RYOLIB_OUTPUT_DIR`,
which defaults to a `dist` folder inside the top-level build directory.
It also copies the assets and the Metal shader library next to the executable,
embeds them when the target is a macOS/iOS application bundle,
and builds the VPK on the Vita.
The `ASSETS_DIR` option is a shorthand for calling `ryolib_add_assets()` first:

```cmake
ryolib_package_app(myproject
    ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets"
)
```

#### Metal

The Metal backend loads `shaders.metallib` at runtime.
`ryolib_package_app()` copies it next to the executable, or embeds it in a macOS/iOS application bundle.
The compiled shader library is found through the `RYOLIB_METALLIB` property on the `ryolib` target.

#### D3D11

The D3D11 backend has no runtime shader files.
Its built-in HLSL shaders (`color`, `texture` and `stencil`) are compiled while building `ryolib`.

If `fxc` is found on the build machine, the shaders are compiled to bytecode and embedded into the library.

Otherwise the raw HLSL source is embedded and compiled at runtime through `d3dcompiler`,
which `ryolib` already links.
This makes cross-compiling from macOS/Linux work without the Windows SDK,
at the cost of a one-time compile when each pipeline is created.
This is a current limitation of the cross-compiling setup, which is intended to be fixed in the future.

Either way the shaders are part of `libryolib`, so nothing extra needs to be copied next to the executable.

#### GXM

The GXM backend loads compiled shader binaries from disk at runtime.
The following six files are required, relative to the application's working directory:

```
shaders/color_v.gxp
shaders/color_f.gxp
shaders/texture_v.gxp
shaders/texture_f.gxp
shaders/stencil_v.gxp
shaders/stencil_f.gxp
```

They are produced by the `ryolib_gxm_shaders` target (using `psp2cgc.exe`, see above)
and written to the `RYOLIB_GXM_SHADER_DIR` property on the `ryolib` target.
`ryolib_package_app()` packs them under `shaders/` automatically:

```cmake
ryolib_package_app(myproject
    VITA_APP_NAME "My Project's Name"
    VITA_TITLEID "ABCD12345"
    VITA_VERSION "12.34"
)
```

This writes `MyGame.vpk` to `RYOLIB_OUTPUT_DIR`,
with the assets declared through `ryolib_add_assets()` packed under `assets/` and the GXM shaders under `shaders/`.

`VITA_APP_NAME` is the application name displayed on the Vita's home screen.

`VITA_TITLEID` is the unique application id used to identify the application on the vita,
only one application with a given id can exist on the Vita at once,
reusing an id causes the application previously occupying it to be replaced with the new one.
It is made up out of 4 letters and 5 digits.

`VITA_VERSION` is the application's version displayed on the vita, it uses the `##.##` format required by the Vita SDK.

## CMake options

| Option | Default | Description |
| --- | --- | --- |
| `RYOLIB_OUTPUT_DIR` | `${CMAKE_BINARY_DIR}/dist` | Folder where `ryolib_package_app()` writes final executables, bundles and VPKs. |
| `RYO_DEBUG` | `ON` in `Debug`, otherwise `OFF` | Compile in debug assertions that abort on invalid use. |
| `RYO_SAFETY` | `ON` | Compile in runtime safety checks that report an error and return instead of crashing. |

## Supported platforms

ryolib has first-class support for macOS and Windows with second-class support for iOS and the Playstation Vita. The project is developed on macOS and manually (for now) tested on macOS and Windows every commit. iOS and Playstation Vita platforms are tested less often, mostly  for more significant commits.

| Platform | Audio | Graphics | Image decoding | Text rasterization | Windowing |
| --- | --- | --- | --- | --- | --- |
| macOS | **Miniaudio**, SDL_Mixer | **Metal** | **stb_image** | **SDL_TTF** | **SDL** |
| iOS / iPadOS | **Miniaudio**, SDL_Mixer | **Metal** | **stb_image** | **SDL_TTF** | **SDL** |
| Windows | **Miniaudio**, SDL_Mixer | **D3D11** | **stb_image** | **SDL_TTF** | **SDL** |
| PlayStation Vita | **SDL_Mixer** | **GXM** | **stb_image** | **SDL_TTF** | **SDL** |

Linux and BSD ports are planned to be added relatively *soon™*, but are not supported yet due to no compatible implemented graphics backends.

Android and Nintendo Switch ports are also planned, but require more work before they can be implemented, specifically missing graphics backends need to be implemented and the build system requires more configuration.