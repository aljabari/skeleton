# Coding Conventions

This project follows the
**[Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)**.

## Key rules

- **Naming:**
  - Types (classes, structs, enums): `PascalCase`
  - Functions and variables: `snake_case`
  - Constants (constexpr / enum values): `kPascalCase`
  - Namespaces: `snake_case`
- **Headers:** Always use traditional `#ifndef` include guards (never `#pragma
  once`). The guard macro must follow the pattern
  `<PROJECT>_<PATH>_<FILE>_H`, e.g. `SKELETON_LIBSKELETON_SKELETON_H`.
- **Includes:** Project headers use quoted paths rooted at the owning target's
  `include/` directory, e.g. `#include "libskeleton/skeleton.h"`.
- **Formatting:** 2-space indentation, 80-column line limit.
- **Comments:** Use `//` for single-line and `/* */` only for large doc blocks.
- **No raw loops** where `std::ranges` or `std::algorithm` can be used instead.
- **Ownership:** Prefer `std::unique_ptr` over raw ownership. Never use
  `new`/`delete` directly.
