// Copyright 2026 aljabari

#include "libskeleton/vulkan/vulkanrenderer.h"

#include <gtest/gtest.h>

#include "libskeleton/renderer.h"

namespace skeleton {
namespace {

// The Vulkan renderer is not implemented yet, so constructing it must fail and
// signal the renderer factory to fall back to another backend.
TEST(VulkanRendererTest, ConstructorThrowsWhenNotImplemented) {
  EXPECT_THROW(VulkanRenderer(), RendererCreationException);
}

}  // namespace
}  // namespace skeleton
