#include "geography.h"

#include <algorithm>
#include <iostream>

#include "constants.h"
#include "grid.h"
#include "random.h"

using namespace std;

constexpr auto kDetail = min(kGeographyWidth, kGeographyLength) >> 2;
constexpr auto kMinDetail = (1ul << 3);

Geography::Geography() {
  Seed();
  Randomize();
}

void Geography::Randomize() {
  height_ = Grid();
  for (size_t i = 0; (kDetail >> i) > kMinDetail; ++i) {
    cout << "Generating Perlin noise with major nodes every " << (kDetail >> i)
         << " values\n";
    height_ += Grid::PerlinNoise(kDetail >> i) / (1 << i);
  }
}
