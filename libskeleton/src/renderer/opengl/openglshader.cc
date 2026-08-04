// Copyright 2026 aljabari

#include "renderer/opengl/openglshader.h"

#include <glad/gl.h>
#include <spdlog/spdlog.h>

#include <cstdint>
#include <cstring>
#include <fstream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

#include <spirv_cross.hpp>
#include <spirv_glsl.hpp>

#include "libskeleton/renderer.h"

namespace skeleton {

namespace {

// Reads a SPIR-V binary file into a sequence of 32-bit words.
std::vector<uint32_t> LoadSpirV(const std::string& path) {
  std::ifstream stream(path, std::ios::binary);
  if (!stream) {
    SPDLOG_ERROR("Failed to open SPIR-V shader \"{}\".", path);
    throw RendererCreationException("Failed to open SPIR-V shader \"" + path +
                                    "\".");
  }
  std::vector<uint8_t> bytes((std::istreambuf_iterator<char>(stream)),
                             std::istreambuf_iterator<char>());
  if (bytes.size() % sizeof(uint32_t) != 0) {
    SPDLOG_ERROR("SPIR-V shader \"{}\" is not 32-bit aligned.", path);
    throw RendererCreationException("SPIR-V shader \"" + path +
                                    "\" is not 32-bit aligned.");
  }
  std::vector<uint32_t> words(bytes.size() / sizeof(uint32_t));
  std::memcpy(words.data(), bytes.data(), bytes.size());
  return words;
}

// Cross-compiles the SPIR-V module at path to desktop GLSL with spirv-cross,
// converting it from the Vulkan coordinate system (clip space y pointing down,
// z in [0, w]) to the OpenGL one (y up, z in [-w, w]).
std::string CrossCompileToGlsl(const std::string& path) {
  spirv_cross::CompilerGLSL compiler(LoadSpirV(path));
  spirv_cross::CompilerGLSL::Options options = compiler.get_common_options();
  options.version = 330;
  options.es = false;
  options.vertex.fixup_clipspace = true;
  options.vertex.flip_vert_y = true;
  compiler.set_common_options(options);
  try {
    return compiler.compile();
  } catch (const std::exception& error) {
    SPDLOG_ERROR("Failed to cross-compile SPIR-V shader \"{}\": {}", path,
                 error.what());
    throw RendererCreationException(
        "Failed to cross-compile SPIR-V shader \"" + path + "\".");
  }
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
      CompileShader(GL_VERTEX_SHADER, CrossCompileToGlsl(vertex_path));
  const GLuint fragment =
      CompileShader(GL_FRAGMENT_SHADER, CrossCompileToGlsl(fragment_path));
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
