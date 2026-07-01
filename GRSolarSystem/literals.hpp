#pragma once

namespace literals {
	constexpr long double operator"" _hour(long double seconds) {
		return seconds * 3600.0;
	}
	constexpr long double operator"" _day(long double seconds) {
		return seconds * 86400.0;
	}
	constexpr long double operator"" _AU(long double metres) {
		return metres * 149597870691.0;
	}
}