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

        glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL );
        shaderProgram.activar();
        glBindVertexArray ( idVAO );
        glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, idIBO );
        glDrawElements ( GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr );

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



    /**
     * @brief Método para crear el VAO del triángulo con atributos de posición y color.
     */
    void PAG::Renderer::creaModelo ( )
    {
        crearModeloEntrelazado();
        //crearModeloNoEntrelazado();
    }

    /**
     * @brief Generar el modelo con VBOs no entrelazados.
     */
    void Renderer::crearModeloNoEntrelazado() {

        GLuint indices[] = { 0, 1, 2 };

        //Determinamos la posicion de los vértices
        GLfloat posiciones[] = {
            -.5f, -.5f, 0.0f,  // Vértice 0 (Esq. inferior izquierda)
             .5f, -.5f, 0.0f,  // Vértice 1 (Esq. inferior derecha)
             .0f,  .5f, 0.0f   // Vértice 2 (Superior centro)
        };

        //Determinamos el color de los vértices
        GLfloat colores[] = {
            1.0f, 0.0f, 0.0f,  // Vértice 0: Rojo
            0.0f, 1.0f, 0.0f,  // Vértice 1: Verde
            0.0f, 0.0f, 1.0f   // Vértice 2: Azul
        };

        //Genera un identificador único
        glGenVertexArrays(1, &idVAO);
        //Activa el VAO
        glBindVertexArray(idVAO);

        //VBO 1: Posiciones (Location 0)
        glGenBuffers(1, &idVBO); //Crea un buffer en la memoria de la GPU
        glBindBuffer(GL_ARRAY_BUFFER, idVBO); //Enlaza el idVBO con el con el buffer activo
        glBufferData(GL_ARRAY_BUFFER, sizeof(posiciones), posiciones, GL_STATIC_DRAW); //Asigna memoria a la GPU
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr); //Definimos como interpreta GPU el eespacio de memoria anterior
        glEnableVertexAttribArray(0); //Habilita el atributo 0

        //VBO 2: Colores (Location 1)
        //Repetimos el mismo procedimiento pero cambiando el array y el index
        glGenBuffers(1, &idVBO_color);
        glBindBuffer(GL_ARRAY_BUFFER, idVBO_color);
        glBufferData(GL_ARRAY_BUFFER, sizeof(colores), colores, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
        glEnableVertexAttribArray(1);

        // IBO: Índices
        glGenBuffers(1, &idIBO); //Buffer de indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIBO); //Enlazamos el buffer
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // Copiamos la secuencia de renderizado
    }

    /**
     * @brief Generar el modelo con VBOs entrelazados.
     */
    void Renderer::crearModeloEntrelazado() {

        GLuint indices[] = { 0, 1, 2 };

        struct Vertice {
            GLfloat x, y, z; // Atributo 0: Posición
            GLfloat r, g, b; // Atributo 1: Color
        };

        Vertice vertices[] = {
            { -.5f, -.5f, 0.0f,   1.0f, 0.0f, 0.0f }, // Vértice 0: Rojo
            {  .5f, -.5f, 0.0f,   0.0f, 1.0f, 0.0f }, // Vértice 1: Verde
            {  .0f,  .5f, 0.0f,   0.0f, 0.0f, 1.0f }  // Vértice 2: Azul
        };

        glGenVertexArrays(1, &idVAO); //Generamos y vinculamos
        glBindVertexArray(idVAO);

        glGenBuffers(1, &idVBO); //Generamos un unico buffer
        glBindBuffer(GL_ARRAY_BUFFER, idVBO); //Lo vinculamos a dicho buffer
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //Le pasamos la informacion completa

        //Definimos la pposición en memoria del atributo, este caso de la posición
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertice), (void*)offsetof(Vertice, x));
        glEnableVertexAttribArray(0);

        //Definimos la pposición en memoria del atributo, este caso del color
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertice), (void*)offsetof(Vertice, r));
        glEnableVertexAttribArray(1);

        //IBO
        glGenBuffers(1, &idIBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    }
}
