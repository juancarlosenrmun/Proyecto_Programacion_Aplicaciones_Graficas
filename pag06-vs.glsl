#version 410 core

layout (location = 0) in vec3 posicion;
layout (location = 1) in vec3 color;

uniform mat4 mModel; // Matriz de modelado
uniform mat4 mVis;   // Matriz de visión
uniform mat4 mProy;  // Matriz de proyección

out vec3 vColor;

void main () {
    vColor = color;
    // Multiplicación en orden correcto: Proyección * Visión * Modelado * Posición
    gl_Position = mProy * mVis * mModel * vec4(posicion, 1.0);
}