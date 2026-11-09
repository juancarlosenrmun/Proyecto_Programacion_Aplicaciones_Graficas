#version 410 core

layout (location = 0) in vec3 posicion;
layout (location = 1) in vec3 normal;

uniform mat4 mModel;
uniform mat4 mVis;
uniform mat4 mProy;

// Bloque de interfaz de salida
out salidaVS {
    vec3 posicionV;
    vec3 normalV;
} salida;

void main() {
    mat4 mModelView = mVis * mModel;
    salida.posicionV = vec3(mModelView * vec4(posicion, 1.0));
    salida.normalV   = transpose(inverse(mat3(mModelView))) * normal;

    gl_Position = mProy * vec4(salida.posicionV, 1.0);
}