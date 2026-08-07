// Copyright 2026 aljabari

#ifndef LIBSKELETON_SCENE_H_
#define LIBSKELETON_SCENE_H_

#include <vector>

#include <entt/entity/registry.hpp>

namespace skeleton {

// A drawable mesh owned by a scene entity: interleaved position (vec3) and
// colour (vec3) floats, authored in the Vulkan coordinate system shared by all
// renderer backends (front faces wind counter-clockwise in the y-down
// framebuffer).
struct MeshComponent {
  std::vector<float> vertices;
};

// An ECS scene: wraps the entt::registry that stores the entities a renderer
// draws. Entities and their components are added and removed through
// Registry().
class Scene {
 public:
  // The registry backing this scene.
  entt::registry& Registry();
  const entt::registry& Registry() const;

 private:
  entt::registry registry_;
};

}  // namespace skeleton

#endif  // LIBSKELETON_SCENE_H_
