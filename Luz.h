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

    struct PropiedadesLuz {
        bool activa = true;
        glm::vec3 Ia = glm::vec3(0.2f, 0.2f, 0.2f);
        glm::vec3 Id = glm::vec3(0.8f, 0.8f, 0.8f);
        glm::vec3 Is = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 p  = glm::vec3(2.0f, 3.0f, 2.0f);    // Posición (mundo)
        glm::vec3 d  = glm::vec3(-1.0f, -1.0f, -1.0f); // Dirección (desde la luz hacia la escena)
        float gamma  = 25.0f;                          // Ángulo de apertura del foco (grados)
        float s      = 8.0f;                           // Exponente de bordes suaves
    };

    class Luz {
    };
}


#endif //PROYECTO_PAG_LUZ_H
