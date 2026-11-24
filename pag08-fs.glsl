#version 410 core

in salidaVS {
    vec3 posicionV;
    vec3 normalV;
} entrada;

uniform vec3 uKa;
uniform vec3 uKd;
uniform vec3 uKs;
uniform float uShininess;

uniform vec3 uIa;
uniform vec3 uId;
uniform vec3 uIs;
uniform vec3 uPosLuzV;
uniform vec3 uDirLuzV;
uniform float uCosGamma;
uniform float uExponenteFoco;

out vec4 colorFragmento;

subroutine vec4 fCalcularColor();
subroutine uniform fCalcularColor uMetodoColorElegido;

vec3 direccionUnitaria(vec3 v) {
    float longitud2 = dot(v, v);
    return longitud2 > 0.0 ? v * inversesqrt(longitud2) : vec3(0.0);
}

vec3 difusaYEspecular(vec3 l) {
    vec3 n = direccionUnitaria(entrada.normalV);
    float cosTheta = dot(n, l);
    if (cosTheta <= 0.0) return vec3(0.0);

    vec3 v = direccionUnitaria(-entrada.posicionV);
    vec3 r = reflect(-l, n);
    float especular = pow(max(dot(r, v), 0.0), uShininess);

    return uId * uKd * cosTheta + uIs * uKs * especular;
}

subroutine(fCalcularColor)
vec4 colorAlambre() {
return vec4(1.0, 0.0, 0.0, 1.0);
}

subroutine(fCalcularColor)
vec4 colorSolido() {
return vec4(uKd, 1.0);
}

subroutine(fCalcularColor)
vec4 luzAmbiente() {
return vec4(uIa * uKa, 1.0);
}

subroutine(fCalcularColor)
vec4 luzPuntual() {
vec3 l = direccionUnitaria(uPosLuzV - entrada.posicionV);
return vec4(difusaYEspecular(l), 1.0);
}

subroutine(fCalcularColor)
vec4 luzDireccional() {
return vec4(difusaYEspecular(-uDirLuzV), 1.0);
}

subroutine(fCalcularColor)
vec4 luzFoco() {
vec3 l = direccionUnitaria(uPosLuzV - entrada.posicionV);
float cosDelta = clamp(dot(-l, uDirLuzV), -1.0, 1.0);
if (cosDelta < uCosGamma) return vec4(0.0, 0.0, 0.0, 1.0);

float factor = uExponenteFoco == 0.0 ? 1.0 : pow(cosDelta, uExponenteFoco);
return vec4(factor * difusaYEspecular(l), 1.0);
}

void main() {
colorFragmento = uMetodoColorElegido();
}
