#version 330 core
// https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows


uniform mat4 model;

in vec3 vtxPos;


void main() {
    gl_Position = model * vec4(vtxPos, 1.0);
}
