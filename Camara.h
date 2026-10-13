/**
* @file Camara.h
 * @author Juan Carlos Enríquez Muñoz
 * @date 2026
 * @brief Declaración de la clase Camara para la gestión de la cámara virtual 3D.
 */

#ifndef PROYECTO_PAG_CAMARA_H
#define PROYECTO_PAG_CAMARA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace PAG {

    enum class TipoMovimiento {
        PAN,
        TILT,
        DOLLY,
        CRANE,
        ORBIT,
        ZOOM
    };

    class Camara {
        private:
            glm::vec3 posicion;     ///< Posición de la cámara (eye)
            glm::vec3 puntoInteres; ///< Punto al que mira la cámara (lookAt)
            glm::vec3 up;           ///< Vector de orientación vertical

            float fovY;             ///< Campo de visión vertical en grados
            float aspecto;          ///< Relación de aspecto (Ancho / Alto)
            float zNear;            ///< Plano cercano de recorte
            float zFar;             ///< Plano lejano de recorte

    };
}


#endif //PROYECTO_PAG_CAMARA_H
