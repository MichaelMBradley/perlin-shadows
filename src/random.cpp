#include "random.h"

#include <ctime>
#include <iostream>
#include <random>

using namespace std;

static random_device device;
static default_random_engine engine(device());

double uniform_double(const double min, const double max) {
  uniform_real_distribution<double> dist(min, max);
  return dist(engine);
}

size_t uniform_size_t(const size_t min, const size_t max) {
  uniform_int_distribution<size_t> dist(min, max);
  return dist(engine);
}

void seed() { seed(static_cast<int>(time(nullptr))); }
void seed(const int seed) { engine.seed(seed); }
