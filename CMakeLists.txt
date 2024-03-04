cmake_minimum_required(VERSION 3.27)
project(ice-simulator)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

link_libraries(-lGL -lglut -lGLEW -lGLU)

add_executable(mountain-simulator
        src/constants.h
        src/final.cpp
        src/geography.cpp
        src/geography.h
        src/grid.cpp
        src/grid.h
        src/noise_math.h
        src/random.cpp
        src/random.h
        src/renderer.cpp
        src/renderer.h
        src/camera.cpp
        src/camera.h
)
