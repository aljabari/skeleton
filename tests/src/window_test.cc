// Copyright 2026 aljabari

#include "libskeleton/window.h"

#include <GLFW/glfw3.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "libskeleton/renderer.h"

namespace skeleton {
namespace {

using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::SaveArg;

class MockRenderer : public Renderer {
 public:
  MOCK_METHOD(void, SetWindowHints, (), (override));
  MOCK_METHOD(void, InitialiseForWindow, (GLFWwindow* window), (override));
  MOCK_METHOD(void, Render, (), (override));
};

TEST(WindowTest, CallsSetWindowHintsBeforeWindowCreation) {
  NiceMock<MockRenderer> renderer;
  testing::InSequence sequence;
  EXPECT_CALL(renderer, SetWindowHints()).WillOnce(Invoke([] {
    EXPECT_EQ(glfwGetCurrentContext(), nullptr);
  }));
  EXPECT_CALL(renderer, InitialiseForWindow(_)).Times(1);

  Window window(800, 600, "Skeleton Test Window", renderer);
}

TEST(WindowTest, PassesCreatedWindowToRenderer) {
  NiceMock<MockRenderer> renderer;
  GLFWwindow* created_window = nullptr;
  EXPECT_CALL(renderer, InitialiseForWindow(_))
      .WillOnce(SaveArg<0>(&created_window));

  Window window(800, 600, "Skeleton Test Window", renderer);

  EXPECT_NE(created_window, nullptr);
}

TEST(WindowTest, IsOpenReturnsTrueAfterCreation) {
  NiceMock<MockRenderer> renderer;
  Window window(800, 600, "Skeleton Test Window", renderer);

  EXPECT_TRUE(window.IsOpen());
}

TEST(WindowTest, IsOpenReturnsFalseWhenCloseRequested) {
  NiceMock<MockRenderer> renderer;
  GLFWwindow* created_window = nullptr;
  EXPECT_CALL(renderer, InitialiseForWindow(_))
      .WillOnce(SaveArg<0>(&created_window));

  Window window(800, 600, "Skeleton Test Window", renderer);
  ASSERT_NE(created_window, nullptr);

  glfwSetWindowShouldClose(created_window, GLFW_TRUE);

  EXPECT_FALSE(window.IsOpen());
}

TEST(WindowTest, MultipleWindowsCanCoexist) {
  NiceMock<MockRenderer> renderer_one;
  NiceMock<MockRenderer> renderer_two;

  Window first(400, 300, "First Test Window", renderer_one);
  Window second(400, 300, "Second Test Window", renderer_two);

  EXPECT_TRUE(first.IsOpen());
  EXPECT_TRUE(second.IsOpen());
}

}  // namespace
}  // namespace skeleton
