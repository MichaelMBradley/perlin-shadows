#pragma once

#include <GL/glew.h>

#include <array>

#include "shader.h"

class Renderable {
 public:
  explicit Renderable(bool);
  ~Renderable();

  void InitGeom(GLuint);
  void Render(GLuint) const;
  void CleanUp();

 protected:
  bool drawTriangles_;

  GLsizei indexCount_{0};
  unsigned int *indices_{nullptr};
  GLsizei vertexCount_{0};
  Vertex *vertices_{nullptr};

 private:
  virtual void SetData() = 0;

  GLuint ebo_{0};
  GLuint vbo_{0};
};
