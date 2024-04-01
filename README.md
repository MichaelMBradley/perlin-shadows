# Real-time Shadow Simulator

Generates several tiles of grids containing Perlin noise, and then renders them with shadows.

Set up using CMake, which Visual Studio should recognize.

## Example Outputs

### Sunset

This image shows a "sunset".
Contains `4x4=16` tiles each containing `(1<<10)^2` points, for a total of 16,777,216 vertices.
Shadows update in real-time with sun position.

<img alt="Detailed heightmap resembling a mountain range is lit softly from the side." src="./img/sunset.png" width="640">
