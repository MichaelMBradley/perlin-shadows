#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <string>

class Shader {
 public:
  Shader(const std::string &, const std::string &);
  ~Shader();

  const bool CopyDataToShader(const glm::mat4 &, const std::string &);
  const bool CopyDataToShader(const glm::vec3 &, const std::string &);
  const bool CopyDataToShader(const glm::vec4 &, const std::string &);

  void PrintStatus() const;

 private:
  GLuint id_;

  static std::string ReadFile(const std::string &);
  static GLuint CompileShader(const std::string &, GLenum);

  static void CheckGLError(const std::string &);
  static void CheckProgramivError(GLuint, GLenum, const std::string &);
  static void CheckShaderivError(GLuint, GLenum, const std::string &);
};
