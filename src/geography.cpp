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

constexpr auto kDetail = min(kGeographyWidth, kGeographyLength) >> 2;
constexpr auto kMinDetail = (1ul << 3);

Geography::Geography() {
  Seed();
  Randomize();
}

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

  auto end_time = chrono::high_resolution_clock::now();
  cout << "Generation time: "
       << duration_cast<milliseconds>(end_time - start_time).count() << "ms\n";
}

void Geography::InitGeom() {
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  // TODO: Release data?
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * kTotalVertices,
               height_.vertices(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glGenBuffers(1, &ebo_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  // TODO: Release data?
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * kTotalIndices,
               Grid::indices(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Geography::Draw() const {
  glColor3f(1, 0, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  glDrawElements(GL_TRIANGLES, kTotalIndices, GL_UNSIGNED_INT, NULL);
  glPointSize(5);
  glDrawArrays(GL_POINTS, 0, kTotalVertices);
}
