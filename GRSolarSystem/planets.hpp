#include "particles.hpp"
#include "constants.hpp"
#include "literals.hpp"
#include <string_view>

using namespace Constants;
using namespace literals;

struct BodyConfig {
    std::string_view name;
    double mass;
    double perihelion;
    double maxVelocity;
};

namespace Catalog {
    constexpr double AU = 149'597'870'700.0;

    constexpr std::array<BodyConfig, 10> bodies = { {
        {"Sun",     1.989e30,  0.0,              0.0},
        {"Mercury", 3.3e23,    0.307_AU,       58980.0},
        {"Venus",   4.87e24,   0.718_AU,       35260.0},
        {"Earth",   5.97e24,   0.983_AU,       30290.0},
        {"Moon",    7.347e22,  0.983_AU + 3.844e8, 1078.0}, // (Relative to system center)
        {"Mars",    6.42e23,   1.382_AU,       26500.0},
        {"Jupiter", 1.898e27,  4.95_AU,        13720.0},
        {"Saturn",  5.68e26,   9.04_AU,        10180.0},
        {"Uranus",  8.68e25,   18.324_AU,      7110.0},
        {"Neptune", 1.02e26,   29.709_AU,      5500.0}
    } };
}

Particles initSolarSystem() {
    Particles system;
    system.resize(Catalog::bodies.size());

    for (size_t i = 0; i < Catalog::bodies.size(); ++i) {
        const auto& config = Catalog::bodies[i];
        system.masses[i] = config.mass;
        system.positions[i] = Vector4d({ 0.0, config.perihelion, 0.0, 0.0 });
        system.velocities[i] = Vector4d({ c, 0.0, 0.0, config.perihelion });
    }

    return system;
}

Particles initSunEarth() {
    Particles system;
    system.resize(2);

    auto const& sun = Catalog::bodies[0];
    system.masses[0] = sun.mass;
    auto const& earth = Catalog::bodies[3];
    system.masses[1] = earth.mass;
    system.positions[1] = Vector4d({ 0.0, earth.perihelion, 0.0, 0.0 });
    system.velocities[1] = Vector4d({ c, 0.0, 0.0, earth.maxVelocity });

    return system;
}