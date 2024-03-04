#pragma once

#include "constants.h"
#include "geography.h"

constexpr auto kBuffer{20};
constexpr auto kInitialScale{3};

enum Render { kHeight, kNormal };

class Renderer {
 public:
  Renderer(int argc, char *argv[]);

  static void DisplayCB();
  static void ReshapeCB(int, int);
  static void KeyboardCB(unsigned char, int, int);

 private:
  static Renderer *window;

  void NormalColor(std::size_t, std::size_t) const;
  void HeightColor(std::size_t, std::size_t) const;

  void Display() const;
  void Reshape(int, int);
  void Keyboard(unsigned char, int, int);

  Render mode_{kNormal};
  Geography geo_;

  int viewport_width_{kGeographyWidth * kInitialScale};
  int viewport_height_{kGeographyLength * kInitialScale};
};
