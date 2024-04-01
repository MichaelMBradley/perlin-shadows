#version 330 core


uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

attribute vec3 vtxPos;
attribute vec3 vtxNormal;

out fragData {
    vec3 worldPos;
    vec3 normal;
} frag;


void main() {
    vec4 worldPos = model * vec4(vtxPos, 1);
    frag.worldPos = worldPos.xyz;
    gl_Position = projection * view * worldPos;

    frag.normal = vtxNormal;
}
