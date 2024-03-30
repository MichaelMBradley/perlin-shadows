#version 330 core

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

attribute vec3 vtxPos;
attribute vec3 vtxNormal;
attribute vec3 vtxColor;

out vec4 fragColor; // Output color to fragment shader

void main() {
    // Calculate the model-view-projection matrix
    mat4 mvp = projection * view * model;

    // Transform the input position
    gl_Position = mvp * vec4(vtxPos, 1);

    // Pass color to the fragment shader
    fragColor = vec4((1 + vtxNormal.x) / 2, (1 + vtxNormal.y) / 2, vtxNormal.z, 1);
}
