#include "geography.h"

#include <chrono>
#include <iostream>
#include <queue>
#include <thread>
#include <vector>

#include "constants.h"
#include "grid.h"

using namespace std;

Geography::Geography(int x, int y) : Renderable(true), x_(x), y_(y) {
  Randomize(false);
  model_ = glm::translate(
      glm::identity<glm::mat4>(),
      glm::vec3(x * (kGeographyShort - 1), y * (kGeographyLong - 1), 0));
}

Geography::~Geography() { CleanUp(); }

// Sums Perlin noise on a variety of factors, then stores the result in results
// Note: Frees factors
void CalculatePerlinNoise(queue<Grid *> *results, int x, int y,
                          vector<size_t> *factors) {
  auto grid = new Grid();
  for (const auto factor : *factors) {
    const auto noise = Grid::PerlinNoise(x, y, kDetail >> factor);
    (*grid) += (*noise) / static_cast<float>(1 << factor);
    delete noise;
  }
  results->push(grid);
  delete factors;
}

void Geography::Randomize(bool load) {
  if (load) {
    CleanUp();
  }

  using std::chrono::duration_cast;
  using std::chrono::milliseconds;

  auto start_time = chrono::high_resolution_clock::now();
  height_ = Grid();

  queue<Grid *> results;
  queue<thread *> threads;
  // Set up one thread for each noise scale
  for (size_t i = 0; i < kMaxThreads; ++i) {
    auto factors = new vector<size_t>();
    for (size_t factor = 0; (kDetail >> factor) > kMinDetail;
         factor += kMaxThreads) {
      factors->push_back(factor + i);
    }
    threads.push(new thread(CalculatePerlinNoise, &results, x_, y_, factors));
  }

  // Wait for thread completion
  while (!threads.empty()) {
    threads.front()->join();
    threads.pop();
  }

  // Add up all noise levels
  while (!results.empty()) {
    auto grid = results.front();
    height_ += *grid;
    delete grid;
    results.pop();
  }

  height_ *= kHeightMultiplier;

  auto end_time = chrono::high_resolution_clock::now();
  cout << "Generation time: "
       << duration_cast<milliseconds>(end_time - start_time).count() << "ms\n";

  if (load) {
    InitGeom();
  }
}

void Geography::SetData() {
  vertexCount_ = kTotalVertices;
  vertices_ = height_.vertices()->data();

  indexCount_ = kTotalIndices;
  indices_ = Grid::indices()->data();
}
