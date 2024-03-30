#pragma once

#include <glm/glm.hpp>

#include "grid.h"

class Geography {
 public:
  Geography();
  ~Geography();

  void Randomize(GLuint);
  void Randomize();

  void InitGeom(GLuint);
  void Draw() const;
  void CleanUp();

 private:
  Grid height_;
  GLuint ebo_{0};
  GLuint vbo_{0};
};
