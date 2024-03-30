#version 330 core


#define RENDER_NORMAL 0
#define RENDER_COLOR 1


uniform int renderMode;

in fragData {
    vec3 worldPos;
    vec3 normal;
    vec3 color;
} frag;

out vec4 outColor;


void main() {
    switch (renderMode) {
        case RENDER_NORMAL:
            outColor = vec4((1 + frag.normal.x) / 2, (1 + frag.normal.y) / 2, frag.normal.z, 1);
            break;
        case RENDER_COLOR:
            outColor = vec4(frag.color, 1);
            break;
    }
}
