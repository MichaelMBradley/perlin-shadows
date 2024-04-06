#version 330 core
// Retrieved 2024/03/31, formatted slightly
// https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows
// By Joey de Vries (https://twitter.com/JoeyDeVriez)
// CC BY 4.0 (https://creativecommons.org/licenses/by/4.0/legalcode)


layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 shadowMatrices[6];

out vec4 FragPos; // FragPos from GS (output per emitvertex)


void main() {
    for (int face = 0; face < 6; ++face) {
        // built-in variable that specifies to which face we render.
        gl_Layer = face;
        for (int i = 0; i < 3; ++i) {
            // for each triangle vertex
            FragPos = gl_in[i].gl_Position;
            gl_Position = shadowMatrices[face] * FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}
