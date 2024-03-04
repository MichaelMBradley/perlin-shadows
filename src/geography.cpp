#include "geography.h"

#include <algorithm>
#include <iostream>

#include "constants.h"
#include "random.h"

using namespace std;

constexpr auto detail = min(geography_width, geography_length) >> 2;
constexpr auto min_detail = (1ul << 3);

Geography::Geography() {
  seed();
  randomize();
}

void Geography::randomize() {
  height = Grid();
  for (size_t i = 0; (detail >> i) > min_detail; ++i) {
    cout << "Generating Perlin noise with major nodes every " << (detail >> i)
         << " values\n";
    height.perlin_noise(detail >> i, 1. / (1 << i));
  }
}
