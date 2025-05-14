#version 450 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aNorm;

void main() {
    gl_Position = vec4(aPos, 1.0, 1.0);
}