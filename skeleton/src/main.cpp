// Copyright 2026 aljabari

#include "libskeleton/window.h"

namespace skeleton {

int Run(int argc, char* argv[]) {
  Window window(800, 600, "Skeleton");
  while (window.IsOpen()) {
    window.PollEvents();
  }
  return 0;
}

}  // namespace skeleton

int main(int argc, char* argv[]) {
  return skeleton::Run(argc, argv);
}
