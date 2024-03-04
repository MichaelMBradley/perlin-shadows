#pragma once

#include <algorithm>
#include <array>
#include <glm/glm.hpp>

#include "constants.h"
#include "noise_math.h"

class Grid {
 public:
  inline double get(const std::size_t x, const std::size_t y) const {
    return data_[x + y * kGeographyWidth];
  }
  inline void set(const std::size_t x, const std::size_t y, double value) {
    data_[x + y * kGeographyWidth] = value;
    //    if (value < minimum_) {
    //      minimum_ = value;
    //    }
    //    if (value > maximum_) {
    //      maximum_ = value;
    //    }
  }

  double sample(double, double) const;
  void put(double, double, double);

  Grid operator+(const Grid&) const;
  void operator+=(const Grid&);
  Grid operator-() const;
  Grid operator-(const Grid&) const;
  void operator-=(const Grid&);
  Grid operator*(double) const;
  void operator*=(double);
  Grid operator/(double) const;
  void operator/=(double);

  inline double min() const { return minimum_; }
  inline double max() const { return maximum_; }

  glm::dvec3 normal_at(std::size_t, std::size_t, double = 1.) const;

  void PerlinNoise(std::size_t, double);

 private:
  inline void CalculateMinMax() {
    const auto min_max = std::minmax_element(data_.begin(), data_.end());
    minimum_ = *min_max.first;
    maximum_ = *min_max.second;
  }

  double minimum_{0.};
  double maximum_{0.};

  std::array<double, kGeographyWidth * kGeographyLength> data_{};
};
