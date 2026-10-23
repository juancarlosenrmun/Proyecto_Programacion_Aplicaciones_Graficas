/**
 * @file Renderer.cpp
 * @author Juan Carlos Enríquez Muñoz
 *
 * @date 22/09/2026
 *
 * @brief Implementación de la clase Renderer
 */

#include <glad/glad.h>

#include "Renderer.h"
#include "GUI.h"

#include <iostream>
#include <fstream>
#include <sstream>

namespace PAG {

    Renderer* Renderer::instance = nullptr;

    /**
     * @brief Constructor por defecto
     */
    Renderer::Renderer()= default;

    /**
     * @brief Destructor por defecto
     */
    Renderer::~Renderer()
    {
        if ( idVBO != 0 )
        { glDeleteBuffers ( 1, &idVBO );
        }
        if ( idIBO != 0 )
        { glDeleteBuffers ( 1, &idIBO );
        }
        if ( idVAO != 0 )
        { glDeleteVertexArrays ( 1, &idVAO );
        }
    }

    /**
     * @brief Consulta del objeto único de la clase
     * @return Referencia a la instancia única de Renderer
     */
    Renderer& Renderer::getInstance() {
        if (!Renderer::instance) {//Inicialización perezosa
            instance = new Renderer();
        }
        return *instance;
    }

    /**
     * @brief Configura el estado inicial de OpenGL (color de fondo por defecto y test de profundidad) y oregistramos el observador.
     */
    void Renderer::inicializar() {
        //Añadimos el escuchador a la lista de GUI
        PAG::GUI::getInstance().addListener(this);
        // Configuración inicial del estado de OpenGL
        glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
        glEnable ( GL_DEPTH_TEST );
        glEnable ( GL_MULTISAMPLE );

    }

    /**
     * @brief Realiza el borrado de los buffers de color y profundidad en cada fotograma.
     */
    void Renderer::refrescar() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // refrescamos el buffer

        if (shaderProgram.estaListo() && idVAO != 0) {
            shaderProgram.activar();

            // Enviamos las matrices de la cámara al shader
            shaderProgram.setUniformMat4("mVis", camara.getMatrizVision());
            shaderProgram.setUniformMat4("mProy", camara.getMatrizProyeccion());

            glBindVertexArray(idVAO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIBO);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);
        }

    }

    /**
     * @brief Establece el área de dibujo en la ventana de OpenGL.
     * @param x Coordenada X de la esquina inferior izquierda.
     * @param y Coordenada Y de la esquina inferior izquierda.
     * @param width Ancho del viewport en píxeles.
     * @param height Alto del viewport en píxeles.
     */
    void Renderer::establecerViewport(int x, int y, int width, int height) {
        glViewport(x, y, width, height);
        if (height > 0) {
            camara.establecerAspecto(static_cast<float>(width) / static_cast<float>(height));
        }
    }

    /**
     * @brief Modifica el color (glClearColor) de la escena.
     * @param r Componente Roja [0.0f, 1.0f].
     * @param g Componente Verde [0.0f, 1.0f].
     * @param b Componente Azul [0.0f, 1.0f].
     * @param a Componente Alfa (Transparencia) [0.0f, 1.0f]. Por defecto es 1.0f.
     */
    void Renderer::establecerColor(float r, float g, float b, float a) {
        glClearColor(r, g, b, a);
    }

    /**
     * @brief Consulta el color configurado actualmente en OpenGL.
     * @param[out] colorActual Array flotante de 4 elementos donde se almacenarán las componentes (R, G, B, A).
     */
    void Renderer::obtenerColor(float colorActual[4]) {
        glGetFloatv(GL_COLOR_CLEAR_VALUE, colorActual);
    }

    /**
     * @brief Muestra en la consola estándar la información técnica del controlador gráfico y versión de OpenGL.
     */
    void Renderer::mostrarInformacionGL() {
        std::cout << "Renderizador: " << glGetString(GL_RENDERER) << std::endl
                  << "Proveedor:    " << glGetString(GL_VENDOR) << std::endl
                  << "Versión GL:   " << glGetString(GL_VERSION) << std::endl
                  << "GLSL:         " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    }

    /**
     * @brief Implementación del callback del patrón Observador.
     * Recibe la notificación cuando se cambia el color desde la GUI.
     */
    void Renderer::notificarCambioColor(float r, float g, float b, float a) {
        // Aplicamos el cambio de color al recibir la notificación
        establecerColor(r, g, b, a);
    }

    void Renderer::notificarCargarShader(const std::string& nombreBase) {
        try {
            // Carga el shader mediante la clase ShaderProgram
            shaderProgram.cargarDesdeArchivos(nombreBase);
            GUI::getInstance().addMensaje("Shader Program '" + nombreBase + "' cargado con éxito.");
        } catch (const std::exception& e) {
            GUI::getInstance().addMensaje(e.what());
        }
    }

    void Renderer::notificarMovimientoCamara(TipoMovimiento tipo, float delta1, float delta2) {

        movimientoActivo = tipo;

        switch (tipo) {
            case TipoMovimiento::PAN:   camara.pan(delta1); break;
            case TipoMovimiento::TILT:  camara.tilt(delta1); break;
            case TipoMovimiento::DOLLY: camara.dolly(delta1, delta2); break;
            case TipoMovimiento::CRANE: camara.crane(delta1); break;
            case TipoMovimiento::ORBIT: camara.orbitar(delta1, delta2); break;
            case TipoMovimiento::ZOOM:  camara.zoom(delta1); break;
        }
    }

    void Renderer::notificarCargarModelo(const std::string& rutaArchivo) {
        try {
            auto nuevoModelo = std::make_unique<Modelo>(rutaArchivo);

            // Asignar una posición ligeramente aleatoria/desplazada para no solapar los modelos
            float offset = static_cast<float>(modelos.size()) * 1.5f;
            nuevoModelo->setPosicion(glm::vec3(offset, 0.0f, 0.0f));

            modelos.push_back(std::move(nuevoModelo));
            GUI::getInstance().addMensaje("Modelo cargado con éxito: " + rutaArchivo);
        } catch (const std::exception& e) {
            GUI::getInstance().addMensaje(e.what());
        }
    }

    void Renderer::notificarEliminarModelo(int indice) {
        if (indice >= 0 && indice < static_cast<int>(modelos.size())) {
            GUI::getInstance().addMensaje("Modelo eliminado: " + modelos[indice]->getNombre());
            modelos.erase(modelos.begin() + indice);
        }
    }

    void Renderer::notificarTransformarModelo(int indice, glm::vec3 pos, glm::vec3 rot, glm::vec3 esc) {
        if (indice >= 0 && indice < static_cast<int>(modelos.size())) {
            modelos[indice]->setPosicion(pos);
            modelos[indice]->setRotacion(rot);
            modelos[indice]->setEscala(esc);
        }
    }

    void Renderer::aplicarMovimientoRaton(float deltaX, float deltaY) {
        float sens = 0.25f;

        switch (movimientoActivo) {
            case TipoMovimiento::PAN:
            case TipoMovimiento::TILT:
                // Combina Pan y Tilt fluidamente al arrastrar
                camara.pan(-deltaX * sens);
                camara.tilt(-deltaY * sens);
                break;

            case TipoMovimiento::DOLLY:
                camara.dolly(deltaX * 0.005f, deltaY * 0.005f);
                break;

            case TipoMovimiento::CRANE:
                camara.crane(-deltaY * 0.005f);
                break;

            case TipoMovimiento::ORBIT:
                camara.orbitar(-deltaY * sens, -deltaX * sens);
                break;

            case TipoMovimiento::ZOOM:
                camara.zoom(deltaY * sens);
                break;
        }
    }
}
