#version 330 core
// Retrieved 2024/03/31, modified to fit program
// https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows
// By Joey de Vries (https://twitter.com/JoeyDeVriez)
// CC BY 4.0 (https://creativecommons.org/licenses/by/4.0/legalcode)


uniform mat4 model;

in vec3 vtxPos;


void main() {
    gl_Position = model * vec4(vtxPos, 1.0);
}
