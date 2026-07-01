#include "particles.hpp"

void Particles::resize(size_t i) {
    positions.resize(i);
    velocities.resize(i);
    masses.resize(i);
    size = i;
}
