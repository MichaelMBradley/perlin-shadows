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
  void GenerateCubeMaps(const Renderable &) const;

  inline GLuint getDepthTexture() const { return depth_; }
  inline void setPosition(const glm::vec3 &pos) {
    pos_ = pos;
    CleanUp();
    InitGeom();
  }

 private:
  void SetData() override;

  glm::vec3 ambient_{0.2};
  glm::vec3 diffuse_{1};
  float specular_{40};
  glm::vec3 pos_;

  Shader shadow_{"shadow.vert", "shadow.frag", "shadow.geom"};
  GLuint fbo_{0};
  GLuint depth_{0};
};
