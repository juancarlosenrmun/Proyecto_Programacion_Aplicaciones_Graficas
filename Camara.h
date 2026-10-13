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

    public:
        Camara();
        Camara(glm::vec3 pos, glm::vec3 lookAt, glm::vec3 vUp, float fov, float aspect, float near, float far);
        ~Camara() = default;

        // Obtención de matrices de transformación
        glm::mat4 getMatrizVision() const;
        glm::mat4 getMatrizProyeccion() const;

        // Getters y Setters
        void establecerAspecto(float nuevoAspecto) { aspecto = nuevoAspecto; }
        float getFovY() const { return fovY; }
        void setFovY(float fov) { fovY = fov; }

        // Movimientos de cámara
        void pan(float anguloGrados);
        void tilt(float anguloGrados);
        void dolly(float desplX, float desplZ);
        void crane(float desplY);
        void orbitar(float deltaLatGrados, float deltaLonGrados);
        void zoom(float deltaFovGrados);
    };
}


#endif //PROYECTO_PAG_CAMARA_H
