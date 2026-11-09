#version 410 core

in salidaVS {
    vec3 posicionV;
    vec3 normalV;
} entrada;

// Material
uniform vec3 uKa;
uniform vec3 uKd;
uniform vec3 uKs;
uniform float uShininess;

out vec4 colorFragmento;

// 1. Declaración del tipo de subrutina y su uniform asociado
subroutine vec4 fCalcularColor();
subroutine uniform fCalcularColor uMetodoColorElegido;

// 2. Implementación A: Modo Alambre (Color Fijo Rojo)
subroutine (fCalcularColor)
vec4 colorAlambre() {
return vec4(1.0, 0.0, 0.0, 1.0); // Rojo para la malla de alambre
}

// 3. Implementación B: Modo Sólido (Color Difuso del Material)
subroutine (fCalcularColor)
vec4 colorSolido() {
return vec4(uKd, 1.0); // Usa el componente Kd del material
}

void main() {
// Evaluación de la subrutina seleccionada desde C++
colorFragmento = uMetodoColorElegido();
}