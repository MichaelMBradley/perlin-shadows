#include <iostream>
#include <stdexcept>

#include "renderer.h"

using namespace std;

int main(int argc, char *argv[]) {
  try {
    Renderer(argc, argv);
  } catch (const runtime_error &error) {
    cerr << error.what() << endl;
  }
  return 0;
}
