// Copyright 2026 aljabari

#include "libskeleton/scene.h"

namespace skeleton {

entt::registry& Scene::Registry() {
  return registry_;
}

const entt::registry& Scene::Registry() const {
  return registry_;
}

}  // namespace skeleton
