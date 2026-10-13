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

     void Camara::pan(float anguloGrados) {
          glm::vec3 n = glm::normalize(posicion - puntoInteres);
          glm::vec3 u = glm::normalize(glm::cross(up, n));
          glm::vec3 v = glm::cross(n, u); // Eje vertical local

          glm::vec3 dir = puntoInteres - posicion;
          dir = glm::rotate(dir, glm::radians(anguloGrados), v);
          puntoInteres = posicion + dir;
     }

     void Camara::tilt(float anguloGrados) {
          glm::vec3 n = glm::normalize(posicion - puntoInteres);
          glm::vec3 u = glm::normalize(glm::cross(up, n)); // Eje horizontal local

          glm::vec3 dir = puntoInteres - posicion;
          dir = glm::rotate(dir, glm::radians(anguloGrados), u);
          puntoInteres = posicion + dir;
     }

     void Camara::dolly(float desplX, float desplZ) {
          glm::vec3 n = glm::normalize(posicion - puntoInteres);
          glm::vec3 u = glm::normalize(glm::cross(up, n));

          glm::vec3 desplazamiento = u * desplX + n * desplZ;
          posicion += desplazamiento;
          puntoInteres += desplazamiento;
     }

     void Camara::crane(float desplY) {
          glm::vec3 desplazamiento = glm::vec3(0.0f, desplY, 0.0f);
          posicion += desplazamiento;
          puntoInteres += desplazamiento;
     }
}
