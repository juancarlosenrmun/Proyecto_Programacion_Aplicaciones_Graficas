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
        private:
            std::string nombre;
            GLuint idVAO = 0;
            GLuint idVBO = 0;
            GLuint idIBO = 0;
            GLsizei numIndices = 0;

            // Transformaciones físicas
            glm::vec3 posicion = glm::vec3(0.0f);
            glm::vec3 rotacion = glm::vec3(0.0f); // Ángulos en grados (X, Y, Z)
            glm::vec3 escala   = glm::vec3(1.0f);

            void crearBuffers(const std::vector<Vertice>& vertices, const std::vector<GLuint>& indices);
        public:
            Modelo(const std::string& rutaArchivo);
            ~Modelo();

            // Deshabilitar copia del mismo modelo
            Modelo(const Modelo&) = delete;
            Modelo& operator=(const Modelo&) = delete;

            void dibujar(const ShaderProgram& shader);

        // Getters / Setters de transformación
            const std::string& getNombre() const { return nombre; }
            glm::vec3 getPosicion() const { return posicion; }
            glm::vec3 getRotacion() const { return rotacion; }
            glm::vec3 getEscala() const { return escala; }

            void setPosicion(const glm::vec3& pos) { posicion = pos; }
            void setRotacion(const glm::vec3& rot) { rotacion = rot; }
            void setEscala(const glm::vec3& esc)   { escala = esc; }
    };
}


#endif //PROYECTO_PAG_MODELO_H
