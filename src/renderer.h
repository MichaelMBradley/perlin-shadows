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
  static void KeyboardUpCB(unsigned char, int, int);
  static void MotionCB(int, int);
  static void PassiveMotionCB(int, int);
  static void TimerCB(int);

 private:
  static Renderer *window;

  void NormalColor(std::size_t, std::size_t) const;
  void HeightColor(std::size_t, std::size_t) const;

  void Display() const;
  void Reshape(int, int);
  void Keyboard(unsigned char, int, int);
  void KeyboardUp(unsigned char, int, int);
  void Motion(int, int);
  void PassiveMotion(int, int);

  void HandleMouseMove(int, int, bool);
  void HandleMovementKey(unsigned char, bool);
  void Tick(int);

  int viewport_width_{kInitialWidth};
  int viewport_height_{kInitialHeight};

  int last_mouse_x_{0};
  int last_mouse_y_{0};

  bool move_forward_{false}, move_backward_{false}, move_left_{false},
      move_right_{false}, move_up_{false}, move_down_{false};

  Render mode_{kNormal};
  Geography geo_;
  Camera camera_{viewport_width_, viewport_height_};
};
