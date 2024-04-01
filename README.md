# Real-time Shadow Simulator

Generates several tiles of grids containing Perlin noise, and then renders them with shadows.

Set up using CMake, which Visual Studio should recognize.

## TODO

* Multiple light sources?
* Dynamically swap cell triangle order to better fit flat surface?
* Adjust normals?
* Fix seams
* Adjust shadow detection bias (based on normal?)

## Example Outputs

### Sunset

This image shows a "sunset".
Contains `4x4` tiles each containing `(1<<9)^2` points, for a total of 4,194,304 vertices.
Shadows update in real-time with sun position.

<img alt="Detailed heightmap resembling a mountain range is lit softly from the side." src="./img/sunset.png" width="640">

### Stress Test

Putting as many vertices as we can on the shader.
Contains `32x64` tiles each containing `(1<<8)^2` points for a total of 134,217,728 vertices.
Renders at barely above 1 frame per second on a 6950XT and uses 7.69GiB of VRAM.

<img alt="Large heightmap from far away, looking almost like an ocean." src="./img/stress.png" width="640">
