#pragma once
#include "maths.hpp"
#include <vector>

struct Particles {
    std::vector<Vector4d> positions;
    std::vector<Vector4d> velocities;
    std::vector<double> masses;
    size_t size = 0;
    void resize(size_t i);
};