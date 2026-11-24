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
    Renderer::Renderer() {
        PropiedadesLuz ambiente;
        ambiente.Ia = glm::vec3(0.45f);
        luces.emplace_back(TipoLuz::AMBIENTE, ambiente);

        PropiedadesLuz puntual;
        puntual.p = glm::vec3(3.0f, 2.0f, 4.0f);
        puntual.Id = glm::vec3(0.65f, 0.55f, 0.45f);
        puntual.Is = glm::vec3(0.4f);
        luces.emplace_back(TipoLuz::PUNTUAL, puntual);

        PropiedadesLuz direccional;
        direccional.d = glm::vec3(1.0f, -0.5f, -1.0f);
        direccional.Id = glm::vec3(0.25f, 0.35f, 0.55f);
        direccional.Is = glm::vec3(0.2f);
        luces.emplace_back(TipoLuz::DIRECCIONAL, direccional);

        PropiedadesLuz foco;
        foco.p = glm::vec3(-3.0f, 3.0f, 4.0f);
        foco.d = -foco.p;
        foco.gamma = 30.0f;
        foco.Id = glm::vec3(0.35f, 0.2f, 0.35f);
        foco.Is = glm::vec3(0.25f);
        luces.emplace_back(TipoLuz::FOCO, foco);
    }

    /**
     * @brief Destructor por defecto
     */
    Renderer::~Renderer() = default;

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

    void Renderer::destruirInstancia() {
        if (instance) {
            GUI::getInstance().removeListener(instance);
            delete instance;
            instance = nullptr;
        }
    }

    /**
     * @brief Configura el estado inicial de OpenGL (color de fondo por defecto y test de profundidad) y oregistramos el observador.
     */
    void Renderer::inicializar() {
        //Añadimos el escuchador a la lista de GUI
        PAG::GUI::getInstance().addListener(this);
        // Configuración inicial del estado de OpenGL
        glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glEnable ( GL_MULTISAMPLE );

    }

    /**
     * @brief Realiza el borrado de los buffers de color y profundidad en cada fotograma.
     */
    void Renderer::refrescar() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (!shaderProgram.estaListo() || modelos.empty()) return;

        shaderProgram.activar();
        const glm::mat4 mVis = camara.getMatrizVision();
        shaderProgram.setUniformMat4("mVis", mVis);
        shaderProgram.setUniformMat4("mProy", camara.getMatrizProyeccion());

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glPolygonMode(GL_FRONT_AND_BACK, modoAlambre ? GL_LINE : GL_FILL);

        const bool admiteLuces = !modoAlambre &&
            shaderProgram.tieneSubrutinaFragmento("luzAmbiente") &&
            shaderProgram.tieneSubrutinaFragmento("luzPuntual") &&
            shaderProgram.tieneSubrutinaFragmento("luzDireccional") &&
            shaderProgram.tieneSubrutinaFragmento("luzFoco");

        bool primeraPasada = true;
        if (admiteLuces) {
            for (const auto& luz : luces) {
                if (!luz.getPropiedades().activa) continue;

                luz.aplicar(mVis, shaderProgram);
                dibujarModelos();

                if (primeraPasada) {
                    primeraPasada = false;
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                    glDepthMask(GL_FALSE);
                }
            }
        }

        if (primeraPasada) {
            const char* subrutina = modoAlambre ? "colorAlambre" : "colorSolido";
            if (shaderProgram.tieneSubrutinaFragmento(subrutina)) {
                shaderProgram.seleccionarSubrutinaFragmento(subrutina);
            }
            dibujarModelos();
        }

        glDepthMask(GL_TRUE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void Renderer::dibujarModelos() {
        for (const auto& modelo : modelos) {
            const Material& material = modelo->getMaterial();
            shaderProgram.setUniformVec3("uKa", material.Ka);
            shaderProgram.setUniformVec3("uKd", material.Kd);
            shaderProgram.setUniformVec3("uKs", material.Ks);
            shaderProgram.setUniformFloat("uShininess", material.shininess);
            modelo->dibujar(shaderProgram);
        }
    }

    void Renderer::notificarAnadirLuz(TipoLuz tipo) {
        luces.emplace_back(tipo);
    }

    void Renderer::notificarEditarLuz(std::size_t indice, TipoLuz tipo,
                                      const PropiedadesLuz& propiedades) {
        luces.at(indice) = Luz(tipo, propiedades);
    }

    void Renderer::notificarEliminarLuz(std::size_t indice) {
        if (indice >= luces.size()) throw std::out_of_range("No existe la luz indicada.");
        luces.erase(luces.begin() + indice);
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
        shaderProgram.cargarDesdeArchivos(nombreBase);
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
