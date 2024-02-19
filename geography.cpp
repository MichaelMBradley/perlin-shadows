#include "geography.h"

using namespace std;

Geography::Geography() {
	const auto detail = min(width, length) >> 2;
	for (size_t i = 0; i < 5; ++i) {
		height += Grid<width, length>::perlin_noise(detail >> i) / (1 << i);
	}
}

void Geography::simulate_drop(size_t x, size_t y) {
	height.set(x, y, height.get(x, y) - 1);
}

void Geography::simulate_random_drop() {
	simulate_drop(width / 2, length / 2);
}

