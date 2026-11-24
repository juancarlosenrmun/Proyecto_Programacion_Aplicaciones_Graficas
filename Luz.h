#ifndef PROYECTO_PAG_LUZ_H
#define PROYECTO_PAG_LUZ_H

#include <glm/glm.hpp>
#include <array>
#include <memory>
#include <string>
#include "ShaderProgram.h"

namespace PAG {

    enum class TipoLuz {
        AMBIENTE = 0,
        PUNTUAL = 1,
        DIRECCIONAL = 2,
        FOCO = 3,
        NUM_TIPOS = 4
    };

    class Luz {
    };
}


#endif //PROYECTO_PAG_LUZ_H
