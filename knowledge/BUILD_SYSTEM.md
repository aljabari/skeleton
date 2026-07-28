# Build System

- **Build tool:** CMake (minimum 3.16)
- **Generator:** Visual Studio (preferred on Windows), Ninja (alternative)
- **C++ standard:** C++20 (required, not optional)

## Targets

| Target       | Type   | Description                         |
|--------------|--------|-------------------------------------|
| `libskeleton`| STATIC | Core library — all logic lives here |
| `skeleton`   | EXECUTABLE | Main entry point, links `libskeleton` |

## How to build

```sh
cmake -B build -G "Visual Studio 18"
cmake --build build --config Release
```

**cpplint** runs automatically before each target is built. If linting finds
any issues, the build fails. You can also run linting standalone:

```sh
cmake --build build --target libskeleton_cpplint
cmake --build build --target skeleton_cpplint
```

or all at once:

```sh
cmake --build build --target skeleton  # triggers both targets
```

## How to open in Visual Studio

```sh
scripts\windows\build_projects.bat
```

This configures the project (if needed) and opens `skeleton.slnx`.

Once inside Visual Studio, build and run via `F5` or `Ctrl+F5`. The executable
will be at `build\skeleton\Release\skeleton.exe` (or `Debug` depending on
configuration).

## Adding a new library target

1. Create a subdirectory with `src/` and `include/<target>/` folders.
2. Add a `CMakeLists.txt` defining the target.
3. Add `add_subdirectory(...)` to the root `CMakeLists.txt`.
