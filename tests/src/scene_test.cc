// Copyright 2026 aljabari

#include "libskeleton/scene.h"

#include <gtest/gtest.h>

#include <vector>

namespace skeleton {
namespace {

TEST(SceneTest, DefaultConstructsWithEmptyRegistry) {
  const Scene scene;

  EXPECT_TRUE(scene.Registry().view<MeshComponent>().empty());
}

TEST(SceneTest, RegistryStoresEntitiesAndComponents) {
  Scene scene;
  const entt::entity triangle = scene.Registry().create();
  scene.Registry().emplace<MeshComponent>(triangle,
                                          std::vector<float>{1.0f, 2.0f});

  ASSERT_EQ(scene.Registry().view<MeshComponent>().size(), 1u);
  const MeshComponent& mesh = scene.Registry().get<MeshComponent>(triangle);
  EXPECT_EQ(mesh.vertices.size(), 2u);
  EXPECT_EQ(mesh.vertices[0], 1.0f);
  EXPECT_EQ(mesh.vertices[1], 2.0f);
}

TEST(SceneTest, ConstRegistryAllowsReading) {
  Scene scene;
  const entt::entity triangle = scene.Registry().create();
  scene.Registry().emplace<MeshComponent>(triangle, std::vector<float>{3.0f});
  const Scene& const_scene = scene;

  EXPECT_EQ(const_scene.Registry().view<MeshComponent>().size(), 1u);
  EXPECT_EQ(const_scene.Registry().get<MeshComponent>(triangle).vertices.size(),
            1u);
}

TEST(SceneTest, MeshComponentDefaultsToEmptyVertices) {
  const MeshComponent mesh;

  EXPECT_TRUE(mesh.vertices.empty());
}

}  // namespace
}  // namespace skeleton
