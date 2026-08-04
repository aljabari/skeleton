// Copyright 2026 aljabari

#include "libskeleton/rendererfactory.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "libskeleton/renderer.h"

namespace skeleton {
namespace {

// Minimal renderer used by tests to identify which backend was created.
class TestRenderer : public Renderer {
 public:
  explicit TestRenderer(RendererBackend backend) : backend_(backend) {}

  RendererBackend GetBackend() const override { return backend_; }
  void CreateContext(const WindowConfig& config) override {
    received_config_ = true;
    config_ = config;
    if (throw_on_context_) {
      throw RendererCreationException("context creation failed");
    }
  }
  void Render() override {}
  GLFWwindow* GetNativeWindow() const override { return nullptr; }

  // When set, CreateContext throws RendererCreationException.
  void SetThrowOnContext(bool throw_on_context) {
    throw_on_context_ = throw_on_context;
  }
  bool ReceivedConfig() const { return received_config_; }
  const WindowConfig& Config() const { return config_; }

 private:
  RendererBackend backend_;
  bool received_config_ = false;
  bool throw_on_context_ = false;
  WindowConfig config_{};
};

// Creates a map with a creator for every backend that always succeeds.
RendererCreatorMap SuccessfulCreators() {
  RendererCreatorMap creators;
  creators.emplace(RendererBackend::kVulkan,
                   [](RenderTarget render_target) {
                     return std::make_unique<TestRenderer>(
                         RendererBackend::kVulkan);
                   });
  creators.emplace(RendererBackend::kOpenGl,
                   [](RenderTarget render_target) {
                     return std::make_unique<TestRenderer>(
                         RendererBackend::kOpenGl);
                   });
  return creators;
}

// Creates a map with a creator for every backend that always fails.
RendererCreatorMap FailingCreators() {
  RendererCreatorMap creators;
  creators.emplace(RendererBackend::kVulkan,
                   [](RenderTarget render_target) -> std::unique_ptr<Renderer> {
                     return nullptr;
                   });
  creators.emplace(RendererBackend::kOpenGl,
                   [](RenderTarget render_target) -> std::unique_ptr<Renderer> {
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
  std::unique_ptr<Renderer> renderer = CreateRenderer(
      RendererBackend::kVulkan, kVulkanThenOpenGl, SuccessfulCreators());

  ASSERT_NE(renderer, nullptr);
  EXPECT_EQ(renderer->GetBackend(), RendererBackend::kVulkan);
}

TEST(RendererFactoryTest, FallsBackWhenPreferredFails) {
  RendererCreatorMap creators = FailingCreators();
  creators[RendererBackend::kOpenGl] = [](RenderTarget render_target) {
    return std::make_unique<TestRenderer>(RendererBackend::kOpenGl);
  };

  std::unique_ptr<Renderer> renderer = CreateRenderer(
      RendererBackend::kVulkan, kVulkanThenOpenGl, creators);

  ASSERT_NE(renderer, nullptr);
  EXPECT_EQ(renderer->GetBackend(), RendererBackend::kOpenGl);
}

TEST(RendererFactoryTest, PreferredWinsOverHigherPriorityFallback) {
  std::unique_ptr<Renderer> renderer = CreateRenderer(
      RendererBackend::kOpenGl, kVulkanThenOpenGl, SuccessfulCreators());

  ASSERT_NE(renderer, nullptr);
  EXPECT_EQ(renderer->GetBackend(), RendererBackend::kOpenGl);
}

TEST(RendererFactoryTest, PreferredNotInPriorityOrderIsStillTriedFirst) {
  std::unique_ptr<Renderer> renderer = CreateRenderer(
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
                   [&vulkan_calls](RenderTarget render_target) {
                     ++vulkan_calls;
                     return std::make_unique<TestRenderer>(
                         RendererBackend::kVulkan);
                   });
  creators.emplace(RendererBackend::kOpenGl,
                   [&opengl_calls](RenderTarget render_target) {
                     ++opengl_calls;
                     return std::make_unique<TestRenderer>(
                         RendererBackend::kOpenGl);
                   });

  CreateRenderer(RendererBackend::kVulkan, kVulkanThenOpenGl,
                             creators);

  EXPECT_EQ(vulkan_calls, 1);
  EXPECT_EQ(opengl_calls, 0);
}

TEST(RendererFactoryTest, SkipsBackendsWithoutACreator) {
  RendererCreatorMap creators;
  creators.emplace(RendererBackend::kOpenGl,
                   [](RenderTarget render_target) {
                     return std::make_unique<TestRenderer>(
                         RendererBackend::kOpenGl);
                   });

  std::unique_ptr<Renderer> renderer = CreateRenderer(
      RendererBackend::kVulkan, kVulkanThenOpenGl, creators);

  ASSERT_NE(renderer, nullptr);
  EXPECT_EQ(renderer->GetBackend(), RendererBackend::kOpenGl);
}

TEST(RendererFactoryTest, ReturnsNullptrWhenEveryBackendFails) {
  std::unique_ptr<Renderer> renderer = CreateRenderer(
      RendererBackend::kVulkan, kVulkanThenOpenGl, FailingCreators());

  EXPECT_EQ(renderer, nullptr);
}

TEST(RendererFactoryTest, ReturnsNullptrWhenNoCreatorsExist) {
  RendererCreatorMap creators;

  std::unique_ptr<Renderer> renderer = CreateRenderer(
      RendererBackend::kVulkan, kVulkanThenOpenGl, creators);

  EXPECT_EQ(renderer, nullptr);
}

TEST(RendererFactoryTest, ForwardsRenderTargetToCreator) {
  RenderTarget forwarded = RenderTarget::kRenderTargetWindow;
  RendererCreatorMap creators;
  creators.emplace(RendererBackend::kVulkan,
                   [&forwarded](RenderTarget render_target) {
                     forwarded = render_target;
                     return std::make_unique<TestRenderer>(
                         RendererBackend::kVulkan);
                   });

  CreateRenderer(RendererBackend::kVulkan, kVulkanThenOpenGl,
                             creators, RenderTarget::kRenderTargetTexture);

  EXPECT_EQ(forwarded, RenderTarget::kRenderTargetTexture);
}

TEST(RendererFactoryTest, FallsBackWhenCreatorThrows) {
  RendererCreatorMap creators;
  creators.emplace(RendererBackend::kVulkan,
                   [](RenderTarget render_target) -> std::unique_ptr<Renderer> {
                     throw RendererCreationException("no Vulkan");
                   });
  creators.emplace(RendererBackend::kOpenGl,
                   [](RenderTarget render_target) {
                     return std::make_unique<TestRenderer>(
                         RendererBackend::kOpenGl);
                   });

  std::unique_ptr<Renderer> renderer = CreateRenderer(
      RendererBackend::kVulkan, kVulkanThenOpenGl, creators);

  ASSERT_NE(renderer, nullptr);
  EXPECT_EQ(renderer->GetBackend(), RendererBackend::kOpenGl);
}

TEST(RendererFactoryTest, ReturnsNullptrWhenEveryCreatorThrows) {
  RendererCreatorMap creators;
  creators.emplace(RendererBackend::kVulkan,
                   [](RenderTarget render_target) -> std::unique_ptr<Renderer> {
                     throw RendererCreationException("no Vulkan");
                   });
  creators.emplace(RendererBackend::kOpenGl,
                   [](RenderTarget render_target) -> std::unique_ptr<Renderer> {
                     throw RendererCreationException("no OpenGL");
                   });

  std::unique_ptr<Renderer> renderer = CreateRenderer(
      RendererBackend::kVulkan, kVulkanThenOpenGl, creators);

  EXPECT_EQ(renderer, nullptr);
}

// Context creation failures (window creation, backend initialisation) happen
// inside CreateRenderer, so they must trigger fallback just like creation
// failures do.
TEST(RendererFactoryTest, FallsBackWhenContextCreationFails) {
  RendererCreatorMap creators;
  creators.emplace(RendererBackend::kVulkan,
                   [](RenderTarget render_target) {
                     auto renderer = std::make_unique<TestRenderer>(
                         RendererBackend::kVulkan);
                     renderer->SetThrowOnContext(true);
                     return renderer;
                   });
  creators.emplace(RendererBackend::kOpenGl,
                   [](RenderTarget render_target) {
                     return std::make_unique<TestRenderer>(
                         RendererBackend::kOpenGl);
                   });

  std::unique_ptr<Renderer> renderer = CreateRenderer(
      RendererBackend::kVulkan, kVulkanThenOpenGl, creators);

  ASSERT_NE(renderer, nullptr);
  EXPECT_EQ(renderer->GetBackend(), RendererBackend::kOpenGl);
}

TEST(RendererFactoryTest, ReturnsNullptrWhenEveryContextCreationFails) {
  RendererCreatorMap creators;
  creators.emplace(RendererBackend::kVulkan,
                   [](RenderTarget render_target) {
                     auto renderer = std::make_unique<TestRenderer>(
                         RendererBackend::kVulkan);
                     renderer->SetThrowOnContext(true);
                     return renderer;
                   });
  creators.emplace(RendererBackend::kOpenGl,
                   [](RenderTarget render_target) {
                     auto renderer = std::make_unique<TestRenderer>(
                         RendererBackend::kOpenGl);
                     renderer->SetThrowOnContext(true);
                     return renderer;
                   });

  std::unique_ptr<Renderer> renderer = CreateRenderer(
      RendererBackend::kVulkan, kVulkanThenOpenGl, creators);

  EXPECT_EQ(renderer, nullptr);
}

// The window configuration is forwarded to the created renderer so window
// creation happens inside context initialisation.
TEST(RendererFactoryTest, ForwardsWindowConfigToContextCreation) {
  RendererCreatorMap creators;
  creators.emplace(RendererBackend::kVulkan,
                   [](RenderTarget render_target) {
                     return std::make_unique<TestRenderer>(
                         RendererBackend::kVulkan);
                   });
  const WindowConfig config{800, 600, "forwarded"};

  std::unique_ptr<Renderer> renderer = CreateRenderer(
      RendererBackend::kVulkan, kVulkanThenOpenGl, creators,
      RenderTarget::kRenderTargetWindow, config);

  ASSERT_NE(renderer, nullptr);
  TestRenderer* test_renderer = dynamic_cast<TestRenderer*>(renderer.get());
  ASSERT_NE(test_renderer, nullptr);
  EXPECT_TRUE(test_renderer->ReceivedConfig());
  EXPECT_EQ(test_renderer->Config().width, config.width);
  EXPECT_EQ(test_renderer->Config().height, config.height);
  EXPECT_STREQ(test_renderer->Config().title, config.title);
}

// Only RendererCreationException triggers fallback; other exceptions must
// propagate to the caller.
TEST(RendererFactoryTest, PropagatesNonCreationExceptions) {
  RendererCreatorMap creators;
  creators.emplace(RendererBackend::kVulkan,
                   [](RenderTarget render_target) -> std::unique_ptr<Renderer> {
                     throw std::runtime_error("unexpected");
                   });

  EXPECT_THROW(CreateRenderer(RendererBackend::kVulkan,
                                          kVulkanThenOpenGl, creators),
               std::runtime_error);
}

// When the preferred backend cannot be created, the platform factory falls
// back to the next backend in the priority order.
TEST(RendererFactoryTest, PlatformFallbackReturnsSupportedBackend) {
  std::unique_ptr<Renderer> renderer =
      CreateRenderer(RendererBackend::kVulkan);

  ASSERT_NE(renderer, nullptr);
  const RendererPriorityList& order = RendererPriorityOrder();
  EXPECT_NE(std::find(order.begin(), order.end(), renderer->GetBackend()),
            order.end());
}

// With no preferred backend, the platform factory returns the first backend
// in the priority order that can be created.
TEST(RendererFactoryTest, CreateRendererReturnsPlatformBackend) {
  std::unique_ptr<Renderer> renderer = CreateRenderer();

  ASSERT_NE(renderer, nullptr);
  const RendererPriorityList& order = RendererPriorityOrder();
  EXPECT_NE(std::find(order.begin(), order.end(), renderer->GetBackend()),
            order.end());
}

}  // namespace
}  // namespace skeleton
