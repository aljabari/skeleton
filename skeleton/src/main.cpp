// Copyright 2026 aljabari

#include <iostream>
#include "libskeleton/skeleton.h"
#include "skeleton/main.h"

namespace skeleton {

int run(int argc, char* argv[]) {
    std::cout << "Hello from skeleton! 2 + 3 = " << add(2, 3) << "\n";
    return 0;
}

}  // namespace skeleton

int main(int argc, char* argv[]) {
    return skeleton::run(argc, argv);
}
