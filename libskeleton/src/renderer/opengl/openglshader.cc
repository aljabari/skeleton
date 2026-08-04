// Copyright 2026 aljabari

#include "renderer/opengl/openglshader.h"

#include <glad/gl.h>

#include <spdlog/spdlog.h>

#include <fstream>
#include <sstream>
#include <string>

namespace skeleton {

namespace {

std::string LoadSource(const std::string& path) {
  std::ifstream stream(path);
  std::ostringstream buffer;
  buffer << stream.rdbuf();
  return buffer.str();
}

GLuint CompileShader(GLenum type, const std::string& source) {
  const GLuint shader = glCreateShader(type);
  const char* source_cstr = source.c_str();
  glShaderSource(shader, 1, &source_cstr, nullptr);
  glCompileShader(shader);

  GLint success = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (success == 0) {
    char info_log[512] = {};
    glGetShaderInfoLog(shader, sizeof(info_log), nullptr, info_log);
    SPDLOG_ERROR("Shader compilation failed:\n{}", info_log);
    return 0;
  }
  return shader;
}

}  // namespace

OpenGlShader::OpenGlShader(const std::string& vertex_path,
                           const std::string& fragment_path)
    : program_(0) {
  const GLuint vertex =
      CompileShader(GL_VERTEX_SHADER, LoadSource(vertex_path));
  const GLuint fragment =
      CompileShader(GL_FRAGMENT_SHADER, LoadSource(fragment_path));
  if (vertex == 0 || fragment == 0) {
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return;
  }

  program_ = glCreateProgram();
  glAttachShader(program_, vertex);
  glAttachShader(program_, fragment);
  glLinkProgram(program_);
  glDeleteShader(vertex);
  glDeleteShader(fragment);

  GLint success = 0;
  glGetProgramiv(program_, GL_LINK_STATUS, &success);
  if (success == 0) {
    char info_log[512] = {};
    glGetProgramInfoLog(program_, sizeof(info_log), nullptr, info_log);
    SPDLOG_ERROR("Shader program link failed:\n{}", info_log);
    glDeleteProgram(program_);
    program_ = 0;
  }
}

OpenGlShader::~OpenGlShader() {
  if (program_ != 0) {
    glDeleteProgram(program_);
  }
}

GLuint OpenGlShader::GetId() const {
  return program_;
}

void OpenGlShader::Use() const {
  if (program_ != 0) {
    glUseProgram(program_);
  }
}

}  // namespace skeleton
