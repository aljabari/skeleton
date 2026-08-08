// Copyright 2026 aljabari

#include "renderer/opengl/openglmesh.h"

#include <cstddef>
#include <vector>

#include "renderer/opengl/gl.h"

namespace skeleton {

namespace {

constexpr GLint kColorLocation = 1;
constexpr GLint kComponentCount = 3;
constexpr GLint kPositionLocation = 0;
constexpr GLsizei kVertexSize = 6;

}  // namespace

OpenGlMesh::OpenGlMesh(const std::vector<float>& vertices)
    : vao_(0), vbo_(0), vertex_count_(vertices.size() / kVertexSize) {
  const GLsizei stride = kVertexSize * static_cast<GLsizei>(sizeof(float));
  const std::size_t color_offset = kComponentCount * sizeof(float);

  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);

  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER,
               static_cast<GLsizeiptr>(vertices.size() * sizeof(float)),
               vertices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(kPositionLocation, kComponentCount, GL_FLOAT, GL_FALSE,
                        stride, reinterpret_cast<const void*>(0));
  glEnableVertexAttribArray(kPositionLocation);

  glVertexAttribPointer(kColorLocation, kComponentCount, GL_FLOAT, GL_FALSE,
                        stride, reinterpret_cast<const void*>(color_offset));
  glEnableVertexAttribArray(kColorLocation);

  glBindVertexArray(0);
}

OpenGlMesh::~OpenGlMesh() {
  glDeleteBuffers(1, &vbo_);
  glDeleteVertexArrays(1, &vao_);
}

void OpenGlMesh::Bind() const {
  glBindVertexArray(vao_);
}

void OpenGlMesh::Draw() const {
  glBindVertexArray(vao_);
  glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertex_count_));
}

}  // namespace skeleton
