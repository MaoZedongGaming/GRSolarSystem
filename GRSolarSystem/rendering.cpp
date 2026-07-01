#include "rendering.hpp"
#include <algorithm>
#include "rlgl.h"
#include "constants.hpp"

// 1 AU = 100 renderUnits

using namespace Constants;

constexpr double toRenderUnits(double dist) {
	return dist * 100.0 / AU;
}

constexpr double toPhysicsUnits(float dist) {
	return dist * AU / 100.0;
}

Vector3 toRenderVector(Vector4d const& dist4) {
	return Vector3{ (float)toRenderUnits(dist4[1]), (float)toRenderUnits(dist4[2]), (float)toRenderUnits(dist4[3]) };
}

Vector4d to4Pos(Vector3 const& pos) {
	Vector4d pos4;
	pos4[1] = toPhysicsUnits(pos.x);
	pos4[2] = toPhysicsUnits(pos.y);
	pos4[3] = toPhysicsUnits(pos.z);
	return pos4;
}

Vector3 warpRenderPos(Vector3 const& renderPos, Particles const& planets) {
	Vector4d pos = to4Pos(renderPos);
	Vector4d displacement{};
	constexpr double visualAmplify = 1e10;
	for (size_t i = 0; i < planets.size; ++i) {
		Vector4d delta = planets.positions[i] - pos;
		double r2 = delta[1] * delta[1] + delta[2] * delta[2] + delta[3] * delta[3];
		double r = sqrt(r2);
		if (r < schwarzschildRadius(planets.masses[i]))
			continue;
		double pullMagnitude = metricAt(1, 1, pos, planets, -1) * visualAmplify/r;
		displacement += delta * pullMagnitude;
	}
	return toRenderVector(pos + displacement);
}

Color gravitationalRedshift(Vector3 const& renderPos, Particles const& planets) {
	double g_00 = metricAt(0, 0, to4Pos(renderPos), planets, -1);
	float timeDilation = (float)abs(g_00);

	float factor = std::clamp(timeDilation, 0.0f, 1.0f);

	unsigned char r = (unsigned char)((1.0f - factor) * 255.0f);
	unsigned char b = (unsigned char)(factor * 255.0f);
	unsigned char g = (unsigned char)(factor * 100.0f);

	return Color{ r, g, b, 255 };
}

void drawSpatialGrid(int length, int slices, Particles const& planets) {
	Vector3 startPos = Vector3{ -length / 2.0f, 0.0f, -length / 2.0f };
	float spacing = (float)length / (float)slices;

	rlBegin(RL_LINES);

	for (int z = 0; z <= slices; z++) {
		for (int x = 0; x <= slices; x++) {
			Vector3 gridPos = Vector3{ startPos.x + spacing * x, 0.0f, startPos.z + spacing * z };
			Vector3 warpedPos = warpRenderPos(gridPos, planets);

			Color redshift = gravitationalRedshift(gridPos, planets);
			rlColor4ub(redshift.r, redshift.g, redshift.b, redshift.a);

			if (z < slices) {
				Vector3 gridForward = gridPos; gridForward.z += spacing;
				Vector3 warpedForward = warpRenderPos(gridForward, planets);
				//DrawLine3D(warpedPos, warpedForward, redshift);
				rlVertex3f(warpedPos.x, warpedPos.y, warpedPos.z);
				rlVertex3f(warpedForward.x, warpedForward.y, warpedForward.z);
			}
			if (x < slices) {
				Vector3 gridRight = gridPos; gridRight.x += spacing;
				Vector3 warpedRight = warpRenderPos(gridRight, planets);
				//DrawLine3D(warpedPos, warpedRight, redshift);
				rlVertex3f(warpedPos.x, warpedPos.y, warpedPos.z);
				rlVertex3f(warpedRight.x, warpedRight.y, warpedRight.z);
			}
		}
	}
	rlEnd();
}

void drawSpatialGrid3D(int length, int slices, Particles const& planets) {
	Vector3 startPos = Vector3{ -length / 2.0f, -length/2.0f , -length / 2.0f };
	float spacing = (float)length / (float)slices;

	rlBegin(RL_LINES);

	for (int y = 0; y <= slices; y++) {
		for (int z = 0; z <= slices; z++) {
			for (int x = 0; x <= slices; x++) {
				Vector3 gridPos = Vector3{ startPos.x + spacing * x, startPos.y + spacing * y, startPos.z + spacing * z };
				Vector3 warpedPos = warpRenderPos(gridPos, planets);

				Color redshift = gravitationalRedshift(gridPos, planets);
				rlColor4ub(redshift.r, redshift.g, redshift.b, redshift.a/2);

				if (z < slices) {
					Vector3 gridForward = gridPos; gridForward.z += spacing;
					Vector3 warpedForward = warpRenderPos(gridForward, planets);
					//DrawLine3D(warpedPos, warpedForward, redshift);
					rlVertex3f(warpedPos.x, warpedPos.y, warpedPos.z);
					rlVertex3f(warpedForward.x, warpedForward.y, warpedForward.z);
				}
				if (x < slices) {
					Vector3 gridRight = gridPos; gridRight.x += spacing;
					Vector3 warpedRight = warpRenderPos(gridRight, planets);
					//DrawLine3D(warpedPos, warpedRight, redshift);
					rlVertex3f(warpedPos.x, warpedPos.y, warpedPos.z);
					rlVertex3f(warpedRight.x, warpedRight.y, warpedRight.z);
				}
				if (y < slices) {
					Vector3 gridUp = gridPos; gridUp.y += spacing;
					Vector3 warpedUp = warpRenderPos(gridUp, planets);
					//DrawLine3D(warpedPos, warpedRight, redshift);
					rlVertex3f(warpedPos.x, warpedPos.y, warpedPos.z);
					rlVertex3f(warpedUp.x, warpedUp.y, warpedUp.z);
				}
			}
		}
	}
	rlEnd();
}