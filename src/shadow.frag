#version 330 core
// Retrieved 2024/03/31, modified to simplify
// https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows
// By Joey de Vries (https://twitter.com/JoeyDeVriez)
// CC BY 4.0 (https://creativecommons.org/licenses/by/4.0/legalcode)


in vec4 FragPos;

uniform vec3 lightPos;
uniform float farPlane;


void main() {
    gl_FragDepth = length(FragPos.xyz - lightPos) / farPlane;
}
