#version 330 core
// https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows


in vec3 vtxPos;


void main() {
    gl_Position = vec4(vtxPos, 1.0);
}
