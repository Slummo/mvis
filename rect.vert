#version 430 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in mat3 aModel;
layout(location = 4) in vec3 aColor;

out vec3 vColor;

void main() {
    vColor = aColor;
    vec3 final = aModel * vec3(aPos, 1.0);
    gl_Position = vec4(final.xy, 0.0, 1.0);
}