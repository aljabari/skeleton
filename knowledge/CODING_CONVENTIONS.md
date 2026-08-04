# Coding Conventions

This project follows the
**[Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)**.

## Key rules

- **Naming:**
  - Types (classes, structs, enums): `PascalCase`
  - Functions: `PascalCase`
  - Variables: `snake_case`
  - Constants (constexpr / enum values): `kPascalCase`
  - Namespaces: `snake_case`
- **File extensions:** All C++ source files must use the `.cc` extension, and
  headers the `.h` extension.
- **Copyright:** Every source file must start with a copyright line:
  `// Copyright <YEAR> aljabari`
- **Headers:** Always use traditional `#ifndef` include guards (never `#pragma
  once`). The guard macro must follow the pattern
  `<PATH>_<FILE>_H_` with a trailing underscore. The path is the target-relative
  location of the file, e.g. `LIBSKELETON_SKELETON_H_` for a file at
  `libskeleton/include/libskeleton/skeleton.h`, and
  `LIBSKELETON_SRC_RENDERER_OPENGL_OPENGLSHADER_H_` for a private header at
  `libskeleton/src/renderer/opengl/openglshader.h`.
- **Includes:** Project headers use quoted paths rooted at the owning target's
  `include/` directory, e.g. `#include "libskeleton/skeleton.h"`.
- **Formatting:** 2-space indentation, 80-column line limit.
- **Comments:** Use `//` for single-line and `/* */` only for large doc blocks.
- **No raw loops** where `std::ranges` or `std::algorithm` can be used instead.
- **Ownership:** Prefer `std::unique_ptr` over raw ownership. Never use
  `new`/`delete` directly.
- **Testing:** All production code must be accompanied by unit tests written
  with **Google Test** (and **Google Mock** where a mock is needed). Tests live
  in the `tests/` directory. See [TESTING.md](TESTING.md).
- **Logging:** All logging uses **spdlog** through the `SPDLOG_*` macros
  (`SPDLOG_DEBUG`/`SPDLOG_INFO`/`SPDLOG_WARN`/`SPDLOG_ERROR`), never the direct
  `spdlog::debug`/etc. function calls and never `std::cout` or `std::cerr`. The
  macros are compiled out below `SPDLOG_ACTIVE_LEVEL`, which the build sets to
  `SPDLOG_LEVEL_TRACE` in Debug builds and `SPDLOG_LEVEL_OFF` in release builds
  (`skeledit` always uses `SPDLOG_LEVEL_TRACE`). Because `libskeleton` is a
  shared static library, its release log level is controlled globally by the
  `SKELETON_ENABLE_LOGGING_IN_RELEASE` CMake option (default `OFF`); enable it
  to compile library logs into release builds, e.g. so the release editor's log
  window shows renderer and window messages. Use `fmt`-style
  `{}` placeholders instead of `<<`-chaining. Log an `error` immediately before
  throwing a `RendererCreationException` so the failure reason is visible even
  when the factory swallows the exception during fallback. All log output uses
  the canonical patterns in `libskeleton/include/libskeleton/logging.h`
  (`kLogPattern` for consoles and the editor log window, `kLogPatternConsole`
  for the coloured `skeledit` console).
