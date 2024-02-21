#include <algorithm>
#include <iostream>

#include "constants.h"
#include "geography.h"
#include "random.h"

using namespace std;

Geography::Geography() {
	seed();
	constexpr auto detail = min(width, length) >> 2;
	for (size_t i = 0; (detail >> i) > 4; ++i) {
		cout << "Generating Perlin noise with major nodes every " << (detail >> i) << " values\n";
		height.perlin_noise(detail >> i, 1. / (1 << i));
	}
}

void Geography::simulate_drop(size_t x, size_t y) {
	height.set(x, y, height.get(x, y) - 1);
}

void Geography::simulate_random_drop() {
	simulate_drop(uniform_size_t(0, width), uniform_size_t(0, length));
}

