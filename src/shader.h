#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <string>

class Shader {
 public:
  Shader(const std::string &, const std::string &);
  ~Shader();

  bool CopyDataToUniform(const glm::dmat4 &, const std::string &) const;
  bool CopyDataToUniform(const glm::dvec3 &, const std::string &) const;
  bool CopyDataToUniform(const glm::dvec4 &, const std::string &) const;

  void PrintStatus() const;

 private:
  GLuint id_;

  static std::string ReadFile(const std::string &);
  static GLuint CompileShader(const std::string &, GLenum);

  static void CheckGLError(const std::string &);
  static void CheckProgramivError(GLuint, GLenum, const std::string &);
  static void CheckShaderivError(GLuint, GLenum, const std::string &);
};
