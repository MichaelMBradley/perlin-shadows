#include "geography.h"

#include <chrono>
#include <iostream>
#include <queue>
#include <thread>
#include <vector>

#include "constants.h"
#include "grid.h"
#include "random.h"

using namespace std;

constexpr auto kDetail = min(kGeographyShort, kGeographyLong) >> 2;
constexpr auto kMinDetail = (1ul << 3);

Geography::Geography() : Renderable(true) {
  Seed();
  Randomize();
}

Geography::~Geography() { CleanUp(); }

// Sums Perlin noise on a variety of factors, then stores the result in results
// Note: Frees factors
void CalculatePerlinNoise(queue<Grid *> *results, vector<size_t> *factors) {
  auto grid = new Grid();
  for (const auto factor : *factors) {
    const auto noise = Grid::PerlinNoise(kDetail >> factor);
    (*grid) += (*noise) / static_cast<float>(1 << factor);
    delete noise;
  }
  results->push(grid);
  delete factors;
}

const size_t kMaxThreads = 4;

void Geography::Randomize(GLuint shaderId) {
  CleanUp();
  Randomize();
  InitGeom(shaderId);
}

void Geography::Randomize() {
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
    threads.push(new thread(CalculatePerlinNoise, &results, factors));
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
}

void Geography::SetData() {
  vertexCount_ = kTotalVertices;
  vertices_ = height_.vertices()->data();

  indexCount_ = kTotalIndices;
  indices_ = Grid::indices()->data();
}
