#pragma once

#include <GL/glew.h>

#include "grid.h"
#include "renderable.h"

class Geography : public Renderable {
 public:
  Geography();
  ~Geography();

  void Randomize(bool);

 protected:
  void SetData() override;

 private:
  Grid height_;
};
