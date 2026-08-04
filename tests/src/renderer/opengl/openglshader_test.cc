// Copyright 2026 aljabari

#include "renderer/opengl/openglshader.h"

#include <glad/gl.h>

#include <GLFW/glfw3.h>
#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

#include <spirv_cross.hpp>
#include <spirv_glsl.hpp>

namespace skeleton {
namespace {

constexpr char kShaderDir[] = SKELETON_SHADER_DIR;

// Loads a SPIR-V binary file into 32-bit words.
std::vector<uint32_t> LoadSpirV(const std::string& path) {
  std::ifstream stream(path, std::ios::binary);
  EXPECT_TRUE(stream) << "Failed to open SPIR-V shader \"" << path << "\"";
  std::vector<uint8_t> bytes((std::istreambuf_iterator<char>(stream)),
                             std::istreambuf_iterator<char>());
  EXPECT_EQ(bytes.size() % sizeof(uint32_t), 0u);
  std::vector<uint32_t> words(bytes.size() / sizeof(uint32_t));
  std::memcpy(words.data(), bytes.data(), bytes.size());
  return words;
}

// Cross-compiles a SPIR-V module to desktop GLSL with the same options the
// OpenGL shader program uses (y flip and clip-space fixup included).
std::string CrossCompileToGlsl(const std::string& path) {
  spirv_cross::CompilerGLSL compiler(LoadSpirV(path));
  spirv_cross::CompilerGLSL::Options options = compiler.get_common_options();
  options.version = 330;
  options.es = false;
  options.vertex.fixup_clipspace = true;
  options.vertex.flip_vert_y = true;
  compiler.set_common_options(options);
  return compiler.compile();
}

class OpenGlShaderTest : public ::testing::Test {
 protected:
  void SetUp() override {
    ASSERT_TRUE(glfwInit());
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    window_ = glfwCreateWindow(640, 480, "Shader Test", nullptr, nullptr);
    ASSERT_NE(window_, nullptr);
    glfwMakeContextCurrent(window_);
    gladLoadGL(glfwGetProcAddress);
  }

  void TearDown() override {
    glfwDestroyWindow(window_);
    glfwTerminate();
  }

  GLFWwindow* window_ = nullptr;
};

TEST_F(OpenGlShaderTest, CompilesAndLinksShaders) {
  const OpenGlShader shader(std::string(kShaderDir) + "/triangle.vert.spv",
                            std::string(kShaderDir) + "/triangle.frag.spv");

  EXPECT_NE(shader.GetId(), 0);
}

TEST_F(OpenGlShaderTest, UseBindsShaderProgram) {
  const OpenGlShader shader(std::string(kShaderDir) + "/triangle.vert.spv",
                            std::string(kShaderDir) + "/triangle.frag.spv");

  shader.Use();

  GLint current_program = 0;
  glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);
  EXPECT_EQ(current_program, static_cast<GLint>(shader.GetId()));
  EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

TEST_F(OpenGlShaderTest, VertexShaderCrossCompilesToOpenGlCoordinates) {
  const std::string source =
      CrossCompileToGlsl(std::string(kShaderDir) + "/triangle.vert.spv");

  // The cross-compiled shader targets the OpenGL 3.3 GLSL dialect and flips
  // the y axis (and the [0, w] to [-w, w] clip depth) so the Vulkan-authored
  // vertices render upright in OpenGL.
  EXPECT_NE(source.find("#version 330"), std::string::npos);
  EXPECT_NE(source.find("gl_Position.y = -gl_Position.y;"),
            std::string::npos);
  EXPECT_NE(source.find("gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;"),
            std::string::npos);
}

TEST_F(OpenGlShaderTest, FragmentShaderCrossCompilesToOpenGl) {
  const std::string source =
      CrossCompileToGlsl(std::string(kShaderDir) + "/triangle.frag.spv");

  EXPECT_NE(source.find("#version 330"), std::string::npos);
}

}  // namespace
}  // namespace skeleton
