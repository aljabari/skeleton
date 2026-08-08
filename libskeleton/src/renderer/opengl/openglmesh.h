// Copyright 2026 aljabari

#ifndef LIBSKELETON_SRC_RENDERER_OPENGL_OPENGLMESH_H_
#define LIBSKELETON_SRC_RENDERER_OPENGL_OPENGLMESH_H_

#include <cstddef>
#include <vector>

#include "renderer/opengl/gl.h"

namespace skeleton {

// RAII wrapper around an OpenGL vertex array object (VAO) and its backing
// vertex buffer object (VBO). The mesh owns its GL resources and frees them
// on destruction.
class OpenGlMesh {
 public:
  explicit OpenGlMesh(const std::vector<float>& vertices);
  ~OpenGlMesh();

  OpenGlMesh(const OpenGlMesh&) = delete;
  OpenGlMesh& operator=(const OpenGlMesh&) = delete;

  void Bind() const;
  void Draw() const;

 private:
  GLuint vao_;
  GLuint vbo_;
  std::size_t vertex_count_;
};

}  // namespace skeleton

#endif  // LIBSKELETON_SRC_RENDERER_OPENGL_OPENGLMESH_H_
