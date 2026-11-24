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
}