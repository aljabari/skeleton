// Copyright 2026 aljabari

#include "libskeleton/rendererfactory.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <memory>
#include <vector>

#include "libskeleton/renderer.h"

namespace skeleton {
namespace {

// Minimal renderer used by tests to identify which backend was created.
class TestRenderer : public Renderer {
 public:
  explicit TestRenderer(RendererBackend backend) : backend_(backend) {}

  RendererBackend GetBackend() const override { return backend_; }
  void SetWindowHints() override {}
  void InitialiseForWindow(GLFWwindow* window) override {}
  void Render() override {}

 private:
  RendererBackend backend_;
};

// Creates a map with a creator for every backend that always succeeds.
RendererCreatorMap SuccessfulCreators() {
  RendererCreatorMap creators;
  creators.emplace(RendererBackend::kVulkan,
                   [](bool render_to_texture) {
                     return std::make_unique<TestRenderer>(
                         RendererBackend::kVulkan);
                   });
  creators.emplace(RendererBackend::kOpenGl,
                   [](bool render_to_texture) {
                     return std::make_unique<TestRenderer>(
                         RendererBackend::kOpenGl);
                   });
  return creators;
}

// Creates a map with a creator for every backend that always fails.
RendererCreatorMap FailingCreators() {
  RendererCreatorMap creators;
  creators.emplace(RendererBackend::kVulkan,
                   [](bool render_to_texture) -> std::unique_ptr<Renderer> {
                     return nullptr;
                   });
  creators.emplace(RendererBackend::kOpenGl,
                   [](bool render_to_texture) -> std::unique_ptr<Renderer> {
                     return nullptr;
                   });
  return creators;
}

const RendererPriorityList kVulkanThenOpenGl = {
    RendererBackend::kVulkan,
    RendererBackend::kOpenGl,
};

TEST(RendererFactoryTest, PriorityOrderContainsOnlySupportedBackends) {
  const RendererPriorityList& order = RendererPriorityOrder();
  EXPECT_FALSE(order.empty());
  for (RendererBackend backend : order) {
    switch (backend) {
      case RendererBackend::kVulkan:
#ifdef SKELETON_TARGET_SUPPORTS_RENDERER_VULKAN
        break;
#else
        ADD_FAILURE() << "Vulkan in priority order but not supported";
        break;
#endif
      case RendererBackend::kOpenGl:
#ifdef SKELETON_TARGET_SUPPORTS_RENDERER_OPENGL
        break;
#else
        ADD_FAILURE() << "OpenGL in priority order but not supported";
        break;
#endif
    }
  }
}

TEST(RendererFactoryTest, PriorityOrderHasNoDuplicateBackends) {
  const RendererPriorityList& order = RendererPriorityOrder();
  for (size_t i = 0; i < order.size(); ++i) {
    for (size_t j = i + 1; j < order.size(); ++j) {
      EXPECT_NE(order[i], order[j]);
    }
  }
}

#ifdef SKELETON_TARGET_SUPPORTS_RENDERER_VULKAN
TEST(RendererFactoryTest, PriorityOrderPrefersVulkanOverOpenGl) {
  const RendererPriorityList& order = RendererPriorityOrder();
  const auto vulkan =
      std::find(order.begin(), order.end(), RendererBackend::kVulkan);
  const auto opengl =
      std::find(order.begin(), order.end(), RendererBackend::kOpenGl);
  ASSERT_NE(vulkan, order.end());
  ASSERT_NE(opengl, order.end());
  EXPECT_LT(vulkan, opengl);
}
#endif

TEST(RendererFactoryTest, CreatesPreferredBackend) {
  std::unique_ptr<Renderer> renderer = CreateRendererWithFallback(
      RendererBackend::kVulkan, kVulkanThenOpenGl, SuccessfulCreators());

  ASSERT_NE(renderer, nullptr);
  EXPECT_EQ(renderer->GetBackend(), RendererBackend::kVulkan);
}

TEST(RendererFactoryTest, FallsBackWhenPreferredFails) {
  RendererCreatorMap creators = FailingCreators();
  creators[RendererBackend::kOpenGl] = [](bool render_to_texture) {
    return std::make_unique<TestRenderer>(RendererBackend::kOpenGl);
  };

  std::unique_ptr<Renderer> renderer = CreateRendererWithFallback(
      RendererBackend::kVulkan, kVulkanThenOpenGl, creators);

  ASSERT_NE(renderer, nullptr);
  EXPECT_EQ(renderer->GetBackend(), RendererBackend::kOpenGl);
}

TEST(RendererFactoryTest, PreferredWinsOverHigherPriorityFallback) {
  std::unique_ptr<Renderer> renderer = CreateRendererWithFallback(
      RendererBackend::kOpenGl, kVulkanThenOpenGl, SuccessfulCreators());

  ASSERT_NE(renderer, nullptr);
  EXPECT_EQ(renderer->GetBackend(), RendererBackend::kOpenGl);
}

TEST(RendererFactoryTest, PreferredNotInPriorityOrderIsStillTriedFirst) {
  std::unique_ptr<Renderer> renderer = CreateRendererWithFallback(
      RendererBackend::kVulkan, {RendererBackend::kOpenGl},
      SuccessfulCreators());

  ASSERT_NE(renderer, nullptr);
  EXPECT_EQ(renderer->GetBackend(), RendererBackend::kVulkan);
}

TEST(RendererFactoryTest, PreferredIsOnlyAttemptedOnce) {
  int vulkan_calls = 0;
  int opengl_calls = 0;
  RendererCreatorMap creators;
  creators.emplace(RendererBackend::kVulkan,
                   [&vulkan_calls](bool render_to_texture) {
                     ++vulkan_calls;
                     return std::make_unique<TestRenderer>(
                         RendererBackend::kVulkan);
                   });
  creators.emplace(RendererBackend::kOpenGl,
                   [&opengl_calls](bool render_to_texture) {
                     ++opengl_calls;
                     return std::make_unique<TestRenderer>(
                         RendererBackend::kOpenGl);
                   });

  CreateRendererWithFallback(RendererBackend::kVulkan, kVulkanThenOpenGl,
                             creators);

  EXPECT_EQ(vulkan_calls, 1);
  EXPECT_EQ(opengl_calls, 0);
}

TEST(RendererFactoryTest, SkipsBackendsWithoutACreator) {
  RendererCreatorMap creators;
  creators.emplace(RendererBackend::kOpenGl,
                   [](bool render_to_texture) {
                     return std::make_unique<TestRenderer>(
                         RendererBackend::kOpenGl);
                   });

  std::unique_ptr<Renderer> renderer = CreateRendererWithFallback(
      RendererBackend::kVulkan, kVulkanThenOpenGl, creators);

  ASSERT_NE(renderer, nullptr);
  EXPECT_EQ(renderer->GetBackend(), RendererBackend::kOpenGl);
}

TEST(RendererFactoryTest, ReturnsNullptrWhenEveryBackendFails) {
  std::unique_ptr<Renderer> renderer = CreateRendererWithFallback(
      RendererBackend::kVulkan, kVulkanThenOpenGl, FailingCreators());

  EXPECT_EQ(renderer, nullptr);
}

TEST(RendererFactoryTest, ReturnsNullptrWhenNoCreatorsExist) {
  RendererCreatorMap creators;

  std::unique_ptr<Renderer> renderer = CreateRendererWithFallback(
      RendererBackend::kVulkan, kVulkanThenOpenGl, creators);

  EXPECT_EQ(renderer, nullptr);
}

TEST(RendererFactoryTest, ForwardsRenderToTextureToCreator) {
  bool forwarded = false;
  RendererCreatorMap creators;
  creators.emplace(RendererBackend::kVulkan,
                   [&forwarded](bool render_to_texture) {
                     forwarded = render_to_texture;
                     return std::make_unique<TestRenderer>(
                         RendererBackend::kVulkan);
                   });

  CreateRendererWithFallback(RendererBackend::kVulkan, kVulkanThenOpenGl,
                             creators, true);

  EXPECT_TRUE(forwarded);
}

// The Vulkan renderer is not implemented yet, so preferring it must fall back
// to another backend instead of returning a Vulkan renderer.
TEST(RendererFactoryTest, PlatformFallbackNeverReturnsVulkan) {
  std::unique_ptr<Renderer> renderer =
      CreateRendererWithFallback(RendererBackend::kVulkan);

  ASSERT_NE(renderer, nullptr);
  EXPECT_NE(renderer->GetBackend(), RendererBackend::kVulkan);
}

}  // namespace
}  // namespace skeleton
