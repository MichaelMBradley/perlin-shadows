#pragma once

#include <GL/glew.h>

#include "grid.h"
#include "renderable.h"

class Geography : public Renderable {
 public:
  Geography(int x, int y);
  ~Geography();

  void Randomize(bool);

  inline float min() const { return height_.min(); }
  inline float max() const { return height_.max(); }

 protected:
  void SetData() override;

 private:
  Grid height_;

  int x_;
  int y_;
};
