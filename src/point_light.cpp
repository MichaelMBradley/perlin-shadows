// Much code relating to the shadows comes from
// https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows

#include "point_light.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>

#include "constants.h"
#include "shader.h"

using namespace std;

PointLight::PointLight(const glm::vec3 &pos) : Renderable(false), pos_(pos) {
  glGenTextures(1, &depth_);
  glBindTexture(GL_TEXTURE_CUBE_MAP, depth_);

  for (auto i = 0; i < 6; ++i) {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                 kShadowMapSize, kShadowMapSize, 0, GL_DEPTH_COMPONENT,
                 GL_FLOAT, nullptr);
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

  glGenFramebuffers(1, &fbo_);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

PointLight::~PointLight() {
  CleanUp();

  glDeleteTextures(1, &depth_);
  glDeleteFramebuffers(1, &fbo_);
}

void PointLight::LoadData(Shader *shader) const {
  if (!shader->CopyDataToUniform(ambient_, "pointLight.ambient")) {
    cerr << "Point light ambient not loaded to shader" << endl;
  }
  if (!shader->CopyDataToUniform(diffuse_, "pointLight.diffuse")) {
    cerr << "Point light diffuse not loaded to shader" << endl;
  }
  if (!shader->CopyDataToUniform(specularPower_, "pointLight.specularPower")) {
    cerr << "Point light specular power not loaded to shader" << endl;
  }
  if (!shader->CopyDataToUniform(specular_, "pointLight.specular")) {
    cerr << "Point light specular not loaded to shader" << endl;
  }
  if (!shader->CopyDataToUniform(pos_, "pointLight.position")) {
    cerr << "Point light position not loaded to shader" << endl;
  }
}

void PointLight::GenerateCubeMaps(
    const vector<Renderable *> &renderables) const {
  auto shadowProj =
      glm::perspective(glm::pi<float>() / 2, 1.0f, kNearPlane, kFarPlane);

  auto shadowTransforms = array<glm::mat4, 6>();
  shadowTransforms[0] =
      shadowProj * glm::lookAt(pos_, pos_ + glm::vec3(1.0, 0.0, 0.0),
                               glm::vec3(0.0, -1.0, 0.0));
  shadowTransforms[1] =
      shadowProj * glm::lookAt(pos_, pos_ + glm::vec3(-1.0, 0.0, 0.0),
                               glm::vec3(0.0, -1.0, 0.0));
  shadowTransforms[2] =
      shadowProj * glm::lookAt(pos_, pos_ + glm::vec3(0.0, 1.0, 0.0),
                               glm::vec3(0.0, 0.0, 1.0));
  shadowTransforms[3] =
      shadowProj * glm::lookAt(pos_, pos_ + glm::vec3(0.0, -1.0, 0.0),
                               glm::vec3(0.0, 0.0, -1.0));
  shadowTransforms[4] =
      shadowProj * glm::lookAt(pos_, pos_ + glm::vec3(0.0, 0.0, 1.0),
                               glm::vec3(0.0, -1.0, 0.0));
  shadowTransforms[5] =
      shadowProj * glm::lookAt(pos_, pos_ + glm::vec3(0.0, 0.0, -1.0),
                               glm::vec3(0.0, -1.0, 0.0));

  glViewport(0, 0, kShadowMapSize, kShadowMapSize);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
  glClear(GL_DEPTH_BUFFER_BIT);
  glUseProgram(shadow_.id());

  shadow_.CopyDataToUniform(6, shadowTransforms.data(), "shadowMatrices");
  shadow_.CopyDataToUniform(pos_, "lightPos");
  shadow_.CopyDataToUniform(kFarPlane, "farPlane");

  for (const auto renderable : renderables) {
    renderable->Render(&shadow_);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PointLight::SetData() {
  vertexCount_ = 1;
  vertices_ = new Vertex({pos_, glm::vec3(0, 0, 1)});

  indexCount_ = 1;
  indices_ = new unsigned int(0);
}
