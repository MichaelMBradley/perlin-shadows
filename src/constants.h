#pragma once

#include <cstdlib>

constexpr std::size_t kGeographyShort{1 << 10};
constexpr std::size_t kGeographyLong{1 << 10};
constexpr float kHeightMultiplier = kGeographyShort * 0.15;

// Calculate the total number of vertices needed for the entire grid
constexpr std::size_t kTotalVertices = kGeographyShort * kGeographyLong;

// 2 triangles * 3 vertices per triangle
constexpr std::size_t kVerticesPerCell = 6;
// Calculate the total number of cells in the grid
constexpr std::size_t kTotalCells =
    (kGeographyShort - 1) * (kGeographyLong - 1);
// Total number of EBO indices
constexpr std::size_t kTotalIndices = kTotalCells * kVerticesPerCell;
