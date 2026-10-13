#version 410 core

layout (location = 0) in vec3 posicion;
layout (location = 1) in vec3 color;

uniform mat4 mVis;
uniform mat4 mProy;

out vec3 vColor;

void main () {
    vColor = color;
    gl_Position = mProy * mVis * vec4(posicion, 1.0);
}