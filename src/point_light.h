#pragma once

#include <GL/glew.h>

#include <glm/vec3.hpp>

#include "renderable.h"
#include "shader.h"

class PointLight : public Renderable {
 public:
  explicit PointLight(const glm::vec3 &);
  ~PointLight();

  void LoadData(Shader *) const;
  void GenerateCubeMaps() const;

 private:
  void SetData() override;

  glm::vec3 ambient_{0.2};
  glm::vec3 diffuse_{1};
  float specular_{40};
  glm::vec3 pos_;
};
