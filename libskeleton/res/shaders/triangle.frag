// Copyright 2026 aljabari
// Vulkan-style GLSL. Compiled to SPIR-V for the Vulkan renderer and
// cross-compiled back to desktop GLSL by spirv-cross for the OpenGL renderer.

#version 450

layout(location = 0) in vec3 vColor;

layout(location = 0) out vec4 FragColor;

void main() {
  FragColor = vec4(vColor, 1.0);
}
