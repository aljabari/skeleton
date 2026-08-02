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
