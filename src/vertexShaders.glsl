#version 330 core

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

in vec4 vtxPos;
attribute vec4 vtxColour;
out vec4 vColour; // Output color to fragment shader

void main() {
    // Calculate the model-view-projection matrix
    mat4 mvp = projection * view * model;

    // Transform the input position
    vec4 position = vec4(inPosition.x, inPosition.y, geo_.height_at(inPosition.x, inPosition.y) * height_multiplier, 1.0);
    gl_Position = mvp * position;

    // Pass color to the fragment shader
    fragColor = vec4(1.0, 1.0, 1.0, 1.0);
}