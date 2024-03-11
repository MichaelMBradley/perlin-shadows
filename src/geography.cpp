#include "geography.h"

#include <chrono>
#include <iostream>
#include <queue>
#include <thread>

#include "constants.h"
#include "grid.h"
#include "random.h"

using namespace std;

constexpr auto kDetail = min(kGeographyWidth, kGeographyLength) >> 2;
constexpr auto kMinDetail = (1ul << 3);

Geography::Geography() {
  Seed();
  Randomize();
}

void CalculatePerlinNoise(queue<Grid *> *results, size_t factor) {
  auto grid = Grid::PerlinNoise(kDetail >> factor);
  (*grid) /= (1 << factor);
  results->push(grid);
}

void Geography::Randomize() {
  using std::chrono::duration_cast;
  using std::chrono::milliseconds;

  auto start_time = chrono::high_resolution_clock::now();
  height_ = Grid();

  queue<Grid *> results;
  queue<thread *> threads;
  // Set up one thread for each noise scale
  for (size_t i = 0; (kDetail >> i) > kMinDetail; ++i) {
    threads.push(new thread(CalculatePerlinNoise, &results, i));
  }

  // Wait for thread completion
  const auto total = threads.size();
  cout << "Perlin noise generations remaining: " << total << "/" << total
       << "\n";
  while (!threads.empty()) {
    threads.front()->join();
    threads.pop();
    cout << "Perlin noise generations remaining: " << threads.size() << "/"
         << total << "\n";
  }

  // Add up all noise levels
  cout << "Summing noises\n";
  while (!results.empty()) {
    height_ += *results.front();
    results.pop();
  }

  auto end_time = chrono::high_resolution_clock::now();
  cout << "Generation time: "
       << duration_cast<milliseconds>(end_time - start_time).count() << "ms\n";
}
