#pragma once

//SI base units, kg m s,

namespace Constants {
	constexpr double c = 299792458.0;
	constexpr double G = 6.6743e-11;
	constexpr double AU = 149597870691.0;

	constexpr double schwarzschildRadius(double mass) {
		return 2 * G * mass / (c * c);
	}
}