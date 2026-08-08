// Copyright 2026 aljabari

#ifndef LIBSKELETON_SRC_RENDERER_OPENGL_GL_H_
#define LIBSKELETON_SRC_RENDERER_OPENGL_GL_H_

// Selects the OpenGL headers for the target platform. Native builds use glad,
// which loads the desktop GL 3.3 core entry points at run time. Emscripten
// builds target WebGL 2 (OpenGL ES 3.0), whose entry points are linked
// directly into the module, so the ES 3.0 headers are used instead.
#if defined(__EMSCRIPTEN__)
#include <GLES3/gl3.h>
#else
#include <glad/gl.h>
#endif

#endif  // LIBSKELETON_SRC_RENDERER_OPENGL_GL_H_
