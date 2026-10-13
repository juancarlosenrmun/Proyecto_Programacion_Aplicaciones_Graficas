/**
* @file Camara.cpp
 * @author Juan Carlos Enríquez Muñoz
 * @date 2026
 * @brief Implementación de la clase Camara con operaciones matemáticas GLM.
 */

#include "Camara.h"
#include <algorithm>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include "Camara.h"

namespace PAG {
     Camara::Camara()
             : posicion(0.0f, 0.0f, 3.0f),
               puntoInteres(0.0f, 0.0f, 0.0f),
               up(0.0f, 1.0f, 0.0f),
               fovY(45.0f),
               aspecto(1024.0f / 576.0f),
               zNear(0.1f),
               zFar(100.0f) {}

     Camara::Camara(glm::vec3 pos, glm::vec3 lookAt, glm::vec3 vUp, float fov, float aspect, float nearP, float farP)
         : posicion(pos), puntoInteres(lookAt), up(vUp), fovY(fov), aspecto(aspect), zNear(nearP), zFar(farP) {}

    glm::mat4 Camara::getMatrizVision() const {
         return glm::lookAt(posicion, puntoInteres, up);
    }

    glm::mat4 Camara::getMatrizProyeccion() const {
         return glm::perspective(glm::radians(fovY), aspecto, zNear, zFar);
    }
}
