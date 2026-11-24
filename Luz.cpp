#include "Luz.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <stdexcept>

namespace PAG {
    namespace {
        const AplicadorLuzAmbiente ambiente;
        const AplicadorLuzPuntual puntual;
        const AplicadorLuzDireccional direccional;
        const AplicadorLuzFoco foco;
        const std::array<const AplicadorLuz*, static_cast<std::size_t>(TipoLuz::NUM_TIPOS)>
            aplicadores{&ambiente, &puntual, &direccional, &foco};
    }

    Luz::Luz(TipoLuz tipo, const PropiedadesLuz& propiedades)
        : tipo(tipo), propiedades(propiedades) {
        if (static_cast<std::size_t>(tipo) >= aplicadores.size()) {
            throw std::invalid_argument("Tipo de luz desconocido.");
        }

        if (tipo == TipoLuz::DIRECCIONAL || tipo == TipoLuz::FOCO) {
            const float longitud2 = glm::dot(propiedades.d, propiedades.d);
            if (!std::isfinite(longitud2) || longitud2 < 1e-12f) {
                throw std::invalid_argument("La direccion de la luz debe ser un vector no nulo y finito.");
            }
        }

        if (tipo == TipoLuz::FOCO &&
            (!std::isfinite(propiedades.gamma) || propiedades.gamma <= 0.0f ||
             propiedades.gamma > 90.0f || !std::isfinite(propiedades.s) || propiedades.s < 0.0f)) {
            throw std::invalid_argument("El foco requiere 0 < gamma <= 90 grados y s >= 0.");
             }
    }
}