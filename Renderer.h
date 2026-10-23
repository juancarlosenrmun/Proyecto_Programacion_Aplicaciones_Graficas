/**
* @file Renderer.h
 * @author Juan Carlos Enríquez Muñoz
 *
 * @date 22/09/2026
 *
 * @brief Declaración de la clase Renderer (Observador concreto).
 */
#ifndef PR02_RENDERER_H
#define PR02_RENDERER_H

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <glm/glm.hpp>

#include<stdexcept>
#include "Listener.h"
#include "ShaderProgram.h"
#include "Camara.h"
#include "Modelo.h"

namespace PAG
{
    /**
    * @brief Esta clase coordina el renderizado de las escenas OpenGL. Se implementa
    * aplicando el patrón de diseño Singleton. Está pensada para que las funciones callback hagan llamadas a sus métodos
     */
    class Renderer: public Listener //Hereda de la Interfaz Listener
    {
        private:
            static Renderer* instance; ///< Puntero al objeto

            Camara camara;
            TipoMovimiento movimientoActivo = TipoMovimiento::PAN; // Movimiento por defecto
            float sensibilidadRaton = 0.5f;

            ShaderProgram shaderProgram;

            // Colección de modelos 3D en la escena
            std::vector<std::unique_ptr<Modelo>> modelos;
            bool modoAlambre = true; ///< Modo de dibujado (GL_LINE/GL_FILL)

            // El constructor es privado para evitar que se cree el objeto desde otros módulos
            Renderer();

        public:
            ~Renderer();
            static Renderer& getInstance();

            void inicializar();
            void refrescar();

            void establecerViewport(int x, int y, int width, int height);

            // Gestión del color de fondo (clear color)
            void establecerColor(float r, float g, float b, float a = 1.0f);
            void obtenerColor(float colorActual[4]);

            void mostrarInformacionGL();

            void notificarCambioColor(float r, float g, float b, float a) override;
            void notificarCargarShader(const std::string &nombreBase) override;
            void notificarMovimientoCamara(TipoMovimiento tipo, float delta1, float delta2 = 0.0f) override;

            void creaModelo();
            void crearModeloNoEntrelazado();
            void crearModeloEntrelazado();

            void aplicarMovimientoRaton(float deltaX, float deltaY);
    };

};





#endif //PR02_RENDERER_H
