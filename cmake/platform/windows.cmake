# Platform capability flags for the target operating system.
# Only included from the root CMakeLists.txt when the target is Windows.

# OpenGL is supported on Windows through the default system driver.
set(SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL TRUE)
