#include "random.h"

#include <ctime>
#include <iostream>
#include <random>

using namespace std;

static random_device device_;
static default_random_engine engine_(device_());

float UniformFloat(float min, float max) {
  uniform_real_distribution<float> dist(min, max);
  return dist(engine_);
}

size_t UniformSize_t(const size_t min, const size_t max) {
  uniform_int_distribution<size_t> dist(min, max);
  return dist(engine_);
}

void Seed() { Seed(static_cast<int>(time(nullptr))); }
void Seed(const int seed) { engine_.seed(seed); }
