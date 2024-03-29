#version 330 core

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

attribute vec4 vtxPos;
attribute vec4 vtxColour;
out vec4 fragColor; // Output color to fragment shader

void main() {
    // Calculate the model-view-projection matrix
    mat4 mvp = projection * view * model;

    // Transform the input position
    gl_Position = mvp * vtxPos;

    // Pass color to the fragment shader
    fragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
