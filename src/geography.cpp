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

constexpr double deposition_rate = 0.1;
constexpr double erosion_rate = 0.1;
constexpr double friction = 0.5;

void Geography::simulate_drop(double x, double y) {
  Vec3 pos(x, y, 0.);
  Vec3 vel(0., 0., 0.);
  double sediment = 0.;
  for (int i = 0; i < 1000; ++i) {
    const auto normal = height.normal_at(static_cast<size_t>(pos.x()),
                                         static_cast<size_t>(pos.y()));

    const auto change = sediment * deposition_rate * normal.y() -
                        erosion_rate * (1 - normal.y());
    sediment += change;
    height.put(pos.x(), pos.y(), change);

    vel = Vec3(vel.x() * friction + normal.x() * friction, vel.y() + normal.y(),
               0.);
    pos += vel;
    if (pos.x() < 0 || pos.x() > static_cast<double>(geography_width) ||
        pos.y() < 0 || pos.y() > static_cast<double>(geography_length)) {
      break;
    }
  }
  height.put(x, y, height.sample(x, y) - 1);
}

void Geography::simulate_random_drop() {
  simulate_drop(uniform_double(0, static_cast<double>(geography_width)),
                uniform_double(0, static_cast<double>(geography_length)));
}
