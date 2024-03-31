#version 330 core
// https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows


in vec4 FragPos;

uniform vec3 lightPos;
uniform float farPlane;


void main() {
    // get distance between fragment and light source
    float lightDistance = length(FragPos.xyz - lightPos);

    // map to [0;1] range by dividing by far_plane
    lightDistance = lightDistance / farPlane;

    // write this as modified depth
    gl_FragDepth = lightDistance;
}
