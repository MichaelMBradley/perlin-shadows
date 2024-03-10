#version 330 core

in vec4 fragColor; // Input color from vertex shader
out vec4 outColor;

void main() {
    // Output the interpolated color
    outColor = fragColor;
}