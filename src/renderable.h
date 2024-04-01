#pragma once

#include <GL/glew.h>

#include <array>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

#include "shader.h"

class Renderable {
 public:
  explicit Renderable(bool);
  ~Renderable();

  void InitGeom();
  void Render(const Shader *) const;
  void CleanUp();

 protected:
  bool drawTriangles_;

  GLsizei indexCount_{0};
  unsigned int *indices_{nullptr};
  GLsizei vertexCount_{0};
  Vertex *vertices_{nullptr};

  glm::mat4 model_{glm::identity<glm::mat4>()};

 private:
  virtual void SetData() = 0;

  GLuint ebo_{0};
  GLuint vbo_{0};
};
