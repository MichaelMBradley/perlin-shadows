#pragma once

#include "camera.h"
#include "constants.h"
#include "geography.h"

enum Render { kHeight, kNormal };

constexpr auto kInitialWidth = 1280;
constexpr auto kInitialHeight = 720;

class Renderer {
 public:
  Renderer(int argc, char *argv[]);

  static void DisplayCB();
  static void ReshapeCB(int, int);
  static void KeyboardCB(unsigned char, int, int);
  static void MotionCB(int, int);
  static void PassiveMotionCB(int, int);

 private:
  static Renderer *window;

  void NormalColor(std::size_t, std::size_t) const;
  void HeightColor(std::size_t, std::size_t) const;

  void Display() const;
  void Reshape(int, int);
  void Keyboard(unsigned char, int, int);
  void Motion(int, int);
  void PassiveMotion(int, int);

  int viewport_width_{kInitialWidth};
  int viewport_height_{kInitialHeight};

  int last_mouse_x_{-1};
  int last_mouse_y_{-1};

  Render mode_{kNormal};
  Geography geo_;
  Camera camera_{viewport_width_, viewport_height_};
};
