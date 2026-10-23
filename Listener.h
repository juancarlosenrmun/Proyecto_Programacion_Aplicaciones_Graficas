/**
* @file Listener.h
 * @author Juan Carlos Enríquez Muñoz
 * @date 25/09/2026
 * @brief Interfaz abstracta para el patrón de diseño Observador.
 */

#ifndef PR02_LISTENER_H
#define PR02_LISTENER_H
#include "Camara.h"


namespace PAG {
    /**
     * @brief Clase interfaz que deben implementar todos los observadores
     * que deseen recibir notificaciones de eventos de la interfaz (GUI).
     */
    class Listener {
        public:
             /**
             * @brief Destructor virtual por defecto.
             */
            virtual ~Listener() = default;

            /**
             * @brief Método callback llamado cuando se modifica el color de fondo en la GUI.
             * @param r Componente Roja [0.0f, 1.0f].
             * @param g Componente Verde [0.0f, 1.0f].
             * @param b Componente Azul [0.0f, 1.0f].
             * @param a Componente Alfa (Transparencia) [0.0f, 1.0f].
             */
            virtual void notificarCambioColor(float r, float g, float b, float a) = 0;

            /**
             * @brief Callback para solicitud de carga de un nuevo Shader Program desde la GUI.
             * @param nombreBase Prefijo de los archivos .glsl.
             */
            virtual void notificarCargarShader(const std::string& nombreBase) {}

            virtual void notificarMovimientoCamara(TipoMovimiento tipo, float delta1, float delta2 = 0.0f) {}

            virtual void notificarCargarModelo(const std::string& rutaArchivo) = 0;
            virtual void notificarEliminarModelo(int indice) = 0;
            virtual void notificarTransformarModelo(int indice, glm::vec3 pos, glm::vec3 rot, glm::vec3 esc) = 0;
    };
    };
}


#endif //PR02_LISTENER_H
