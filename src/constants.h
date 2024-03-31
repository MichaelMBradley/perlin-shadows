#pragma once

#include <GL/glew.h>

constexpr GLsizei kShadowMapSize{1024};

constexpr std::size_t kGeographyShort{1 << 9};
constexpr std::size_t kGeographyLong{kGeographyShort << 0};
constexpr float kHeightMultiplier{kGeographyShort * 0.15};

constexpr float kNearPlane{0.1};
constexpr float kFarPlane{kGeographyLong * 3};
constexpr float kFOV{45};

constexpr auto kFPS = 60;

// Calculate the total number of vertices_ needed for the entire grid
constexpr std::size_t kTotalVertices{kGeographyShort * kGeographyLong};

// 2 drawTriangles_ * 3 vertices_ per triangle
constexpr std::size_t kVerticesPerCell{6};
// Calculate the total number of cells in the grid
constexpr std::size_t kTotalCells{(kGeographyShort - 1) * (kGeographyLong - 1)};
// Total number of EBO indices
constexpr std::size_t kTotalIndices{kTotalCells * kVerticesPerCell};
