cmake_minimum_required(VERSION 3.27)
project(perlin-shadows)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

link_libraries(-lGL -lglut -lGLEW -lGLU)

add_executable(perlin-shadows
        src/constants.h
        src/final.cpp
        src/geography.cpp
        src/geography.h
        src/grid.cpp
        src/grid.h
        src/noise_math.h
        src/renderer.cpp
        src/renderer.h
        src/camera.cpp
        src/camera.h
        src/shader.cpp
        src/shader.h
        src/point_light.cpp
        src/point_light.h
        src/renderable.cpp
        src/renderable.h
)
