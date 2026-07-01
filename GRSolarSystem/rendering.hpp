#include "raylib.h"
#include "general_relativity.hpp"
#include "literals.hpp"

constexpr double toRenderUnits(double dist);

constexpr double toPhysicsUnits(float dist);

Vector3 toRenderVector(Vector4d const& dist4);

Vector4d to4Pos(Vector3 const& pos);

Color gravitationalRedshift(Vector3 const& renderPos, Particles const& particles);

void drawSpatialGrid(int length, int slices, Particles const& planets);

void drawSpatialGrid3D(int length, int slices, Particles const& planets);