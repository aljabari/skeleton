// Copyright 2026 aljabari
// Vulkan-style GLSL. Compiled to SPIR-V for the Vulkan renderer and
// cross-compiled back to desktop GLSL (with a y flip) by spirv-cross for the
// OpenGL renderer.

#version 450

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColor;

layout(location = 0) out vec3 vColor;

void main() {
  gl_Position = vec4(aPosition, 1.0);
  vColor = aColor;
}
