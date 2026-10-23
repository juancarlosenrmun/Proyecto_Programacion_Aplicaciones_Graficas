//
// Created by Juank on 23/10/2026.
//
#include "Modelo.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stdexcept>

namespace PAG {

    Modelo::Modelo(const std::string& rutaArchivo) {
        // Extraer el nombre del archivo para la GUI
        size_t posSlash = rutaArchivo.find_last_of("/\\");
        nombre = (posSlash != std::string::npos) ? rutaArchivo.substr(posSlash + 1) : rutaArchivo;

        Assimp::Importer importador;
        const aiScene* escena = importador.ReadFile(
            rutaArchivo,
            aiProcess_JoinIdenticalVertices |
            aiProcess_Triangulate |
            aiProcess_GenSmoothNormals
        );

        if (!escena || escena->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !escena->mRootNode) {
            throw std::runtime_error("Error al cargar modelo con Assimp: " + std::string(importador.GetErrorString()));
        }

        std::vector<Vertice> verticesTotales;
        std::vector<GLuint> indicesTotales;
        unsigned int desplazamientoVertices = 0;

        // Recorremos todas las mallas de la escena importada para unificarlas en un único VAO
        for (unsigned int m = 0; m < escena->mNumMeshes; ++m) {
            aiMesh* malla = escena->mMeshes[m];

            for (unsigned int i = 0; i < malla->mNumVertices; ++i) {
                Vertice vert;
                vert.posicion = glm::vec3(malla->mVertices[i].x, malla->mVertices[i].y, malla->mVertices[i].z);

                // Asignamos un color basado en la posición o normales si no hay materiales
                vert.color = glm::vec3(0.7f, 0.7f, 0.7f);
                verticesTotales.push_back(vert);
            }

            for (unsigned int f = 0; f < malla->mNumFaces; ++f) {
                aiFace cara = malla->mFaces[f];
                for (unsigned int j = 0; j < cara.mNumIndices; ++j) {
                    indicesTotales.push_back(desplazamientoVertices + cara.mIndices[j]);
                }
            }

            desplazamientoVertices += malla->mNumVertices;
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