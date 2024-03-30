#pragma once

#include <glm/glm.hpp>

#include "grid.h"

class Geography {
 public:
  Geography();
  ~Geography();

  void Randomize();

  inline float height_at(const std::size_t x, const std::size_t y) const {
    return height_.get(x, y);
  }
  inline glm::vec3 normal_at(const std::size_t x, const std::size_t y,
                             const float amplification = 1) const {
    return height_.normal_at(x, y, amplification);
  }

  inline float min_height() const { return height_.min(); }
  inline float max_height() const { return height_.max(); }

  void InitGeom();
  void Draw() const;

  inline GLuint vbo() const { return vbo_; }

 private:
  Grid height_;
  GLuint ebo_{0};
  GLuint vbo_{0};
};
