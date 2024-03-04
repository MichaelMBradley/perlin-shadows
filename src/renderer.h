#pragma once

#include "constants.h"
#include "geography.h"

constexpr auto buffer{20};
constexpr auto initial_scale{3};

enum Render { height, normal };

class Renderer {
 public:
  Renderer(int argc, char *argv[]);

  static void displayCB();
  static void reshapeCB(int, int);
  static void keyboardCB(unsigned char, int, int);

 private:
  static Renderer *window;

  void normal_color(std::size_t, std::size_t) const;
  void height_color(std::size_t, std::size_t) const;

  void display() const;
  void reshape(int, int);
  void keyboard(unsigned char, int, int);

  Render mode{normal};
  Geography geo;

  int viewport_width{geography_width * initial_scale};
  int viewport_height{geography_length * initial_scale};
};
