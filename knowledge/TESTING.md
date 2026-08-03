# Testing

This project uses **Google Test** (with **Google Mock**) for unit tests.

## Framework

- **Google Test** — test framework (`gtest`)
- **Google Mock** — mocking framework (`gmock`), bundled with Google Test

## Build gate

Tests are built only when the `SKELETON_BUILD_TESTS` CMake option is enabled:

```sh
cmake -B build -DSKELETON_BUILD_TESTS=ON
```

The option **defaults to `ON`**. Set it to `OFF` to compile without tests:

```sh
cmake -B build -DSKELETON_BUILD_TESTS=OFF
```

## Layout

Tests live in the `tests/` directory and mirror the `libskeleton/src/` layout:
each implementation file has a matching `*_test.cc` in the same relative
location. Currently:

| Target              | Sources                                        | Covers                                           |
|---------------------|------------------------------------------------|--------------------------------------------------|
| `libskeleton_tests` | `tests/src/window_test.cc`                     | `Window` behaviour, verified with a mock renderer |
|                     | `tests/src/renderer/opengl/openglrenderer_test.cc` | OpenGL renderer window hints, GL context initialisation, and triangle rendering |
|                     | `tests/src/renderer/opengl/openglshader_test.cc` | `OpenGlShader` compilation/linking and program binding |
|                     | `tests/src/renderer/opengl/openglmesh_test.cc` | `OpenGlMesh` VAO binding and error-free drawing |
|                     | `tests/src/renderer/opengl/opengltexture_test.cc` | `OpenGlTexture` allocation, binding, and cleanup |
|                     | `tests/src/renderer/vulkan/vulkanrenderer_test.cc` | Vulkan renderer window hints |

Tests for the private OpenGL classes (`OpenGlShader`, `OpenGlMesh`,
`OpenGlTexture`) include their headers via the `libskeleton/src` include
directory, which the tests target adds privately. `SKELETON_RES_DIR` (exported
publicly by `libskeleton`) points tests at the real shader files under
`libskeleton/res/shaders`.

## Running tests

Build and run through CTest:

```sh
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

or run the test executable directly:

```sh
build\tests\Release\libskeleton_tests.exe
```

## Policy

**All production code must be accompanied by unit tests.** New classes and
functions should add coverage in `tests/` before the work is considered done.
