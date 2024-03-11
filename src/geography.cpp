#include "geography.h"

#include <algorithm>
#include <chrono>
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
  using std::chrono::duration_cast;
  using std::chrono::milliseconds;

  height_ = Grid();
  auto start_time = chrono::high_resolution_clock::now();
  for (size_t i = 0; (kDetail >> i) > kMinDetail; ++i) {
    cout << "Generating Perlin noise with major nodes every " << (kDetail >> i)
         << " values\n";
    height_ += Grid::PerlinNoise(kDetail >> i) / (1 << i);
  }
  auto end_time = chrono::high_resolution_clock::now();
  cout << "Generation time: "
       << duration_cast<milliseconds>(end_time - start_time).count() << "ms\n";
}
