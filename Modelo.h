//
// Created by Juank on 23/10/2026.
//

#ifndef PROYECTO_PAG_MODELO_H
#define PROYECTO_PAG_MODELO_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include "ShaderProgram.h"

namespace PAG {

    struct Vertice {
        glm::vec3 posicion;
        glm::vec3 color; // Color asignado/interpolado
    };

    class Modelo {
    };
}


#endif //PROYECTO_PAG_MODELO_H
