//
// Created by Juank on 23/10/2026.
//
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "Modelo.h"
#include <stdexcept>

namespace PAG {

    Modelo::Modelo(const std::string& rutaArchivo) {
        size_t posSlash = rutaArchivo.find_last_of("/\\");
        nombre = (posSlash != std::string::npos) ? rutaArchivo.substr(posSlash + 1) : rutaArchivo;

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        // Carga del archivo OBJ
        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, rutaArchivo.c_str());

        if (!ret) {
            throw std::runtime_error("Error al cargar OBJ: " + err + warn);
        }

        std::vector<Vertice> verticesTotales;
        std::vector<GLuint> indicesTotales;
        GLuint idx = 0;

        // Recorremos todas las formas (shapes) del modelo
        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                Vertice vert;
                vert.posicion = glm::vec3(
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                );

                // Si el archivo OBJ tiene información de normales, las usamos para generar un color RGB
                if (!attrib.normals.empty() && index.normal_index >= 0) {
                    glm::vec3 normal(
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]
                    );
                    // Mapeamos la normal del rango [-1.0, 1.0] al rango de color RGB [0.0, 1.0]
                    vert.color = glm::normalize(normal) * 0.5f + glm::vec3(0.5f);
                } else {
                    // Si no tiene normales, usamos un color basado en la posición
                    vert.color = glm::abs(glm::normalize(vert.posicion));
                }

                verticesTotales.push_back(vert);
                indicesTotales.push_back(idx++);
            }
        }

        crearBuffers(verticesTotales, indicesTotales);
    }

    Modelo::~Modelo() {
        if (idVBO != 0) glDeleteBuffers(1, &idVBO);
        if (idIBO != 0) glDeleteBuffers(1, &idIBO);
        if (idVAO != 0) glDeleteVertexArrays(1, &idVAO);
    }

    void Modelo::crearBuffers(const std::vector<Vertice>& vertices, const std::vector<GLuint>& indices) {
        numIndices = static_cast<GLsizei>(indices.size());

        glGenVertexArrays(1, &idVAO);
        glBindVertexArray(idVAO);

        glGenBuffers(1, &idVBO);
        glBindBuffer(GL_ARRAY_BUFFER, idVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertice), vertices.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &idIBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

        // Atributo 0: Posición
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertice), (void*)offsetof(Vertice, posicion));
        glEnableVertexAttribArray(0);

        // Atributo 1: Color
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertice), (void*)offsetof(Vertice, color));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    glm::mat4 Modelo::getMatrizModelado() const {
        glm::mat4 M = glm::mat4(1.0f);
        M = glm::translate(M, posicion);
        M = glm::rotate(M, glm::radians(rotacion.x), glm::vec3(1.0f, 0.0f, 0.0f));
        M = glm::rotate(M, glm::radians(rotacion.y), glm::vec3(0.0f, 1.0f, 0.0f));
        M = glm::rotate(M, glm::radians(rotacion.z), glm::vec3(0.0f, 0.0f, 1.0f));
        M = glm::scale(M, escala);
        return M;
    }

    void Modelo::dibujar(const ShaderProgram& shader) {
        if (idVAO != 0 && numIndices > 0) {
            shader.setUniformMat4("mModel", getMatrizModelado());
            glBindVertexArray(idVAO);
            glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
        }
    }

}