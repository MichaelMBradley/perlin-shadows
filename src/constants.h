#pragma once

#include <cstdlib>

constexpr std::size_t kGeographyWidth{1 << 10};
constexpr std::size_t kGeographyLength{1 << 10};

constexpr std::size_t kMinGeographyMagnitude =
    kGeographyWidth > kGeographyLength ? kGeographyLength : kGeographyWidth;

// Calculate the total number of vertices needed for the entire grid
constexpr std::size_t kTotalVertices = kGeographyWidth * kGeographyLength;

// 2 triangles * 3 vertices per triangle
constexpr std::size_t kVerticesPerCell = 6;
// Calculate the total number of cells in the grid
constexpr std::size_t kTotalCells =
    (kGeographyWidth - 1) * (kGeographyLength - 1);
// Total number of EBO indices
constexpr std::size_t kTotalIndices = kTotalCells * kVerticesPerCell;
