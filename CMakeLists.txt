cmake_minimum_required(VERSION 3.27)
project(mountain-simulator)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

link_libraries(-lGL -lglut -lGLEW -lGLU)

add_executable(mountain-simulator
        src/final.cpp
        src/constants.h
        src/geography.cpp
        src/geography.h
        src/grid.cpp
        src/grid.h
        src/noise_math.h
        src/random.cpp
        src/random.h
        src/vec.cpp
        src/vec.h)
