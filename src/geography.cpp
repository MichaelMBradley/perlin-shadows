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

  auto end_time = chrono::high_resolution_clock::now();
  cout << "Generation time: "
       << duration_cast<milliseconds>(end_time - start_time).count() << "ms\n";
}

void Geography::InitGeom(GLuint shaderId) {
  glGenBuffers(1, &vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * kTotalVertices,
               height_.vertices(), GL_STATIC_DRAW);

  GLuint attribLoc = glGetAttribLocation(shaderId, "vtxPos");
  if (attribLoc != -1) {
    glEnableVertexAttribArray(attribLoc);
    glVertexAttribPointer(attribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, position));
  } else {
    cerr << "Not loading vtxPos" << endl;
  }

  attribLoc = glGetAttribLocation(shaderId, "vtxNormal");
  if (attribLoc != -1) {
    glEnableVertexAttribArray(attribLoc);
    glVertexAttribPointer(attribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, normal));
  } else {
    cerr << "Not loading vtxNormal" << endl;
  }

  attribLoc = glGetAttribLocation(shaderId, "vtxColor");
  if (attribLoc != -1) {
    glEnableVertexAttribArray(attribLoc);
    glVertexAttribPointer(attribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, color));
  } else {
    cerr << "Not loading vtxColor" << endl;
  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glGenBuffers(1, &ebo_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * kTotalIndices,
               Grid::indices(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Geography::Draw() const {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  glDrawElements(GL_TRIANGLES, kTotalIndices, GL_UNSIGNED_INT, nullptr);
  // Optionally draw points and vertices
  //  glPointSize(5);
  //  glDrawArrays(GL_POINTS, 0, kTotalVertices);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Geography::CleanUp() {
  glDeleteBuffers(1, &vbo_);
  glDeleteBuffers(1, &ebo_);
}
