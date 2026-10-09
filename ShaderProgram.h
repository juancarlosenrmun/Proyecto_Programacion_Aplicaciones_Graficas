//
// Created by Juank on 09/10/2026.
//

#ifndef PROYECTO_PAG_SHADERPROGRAM_H
#define PROYECTO_PAG_SHADERPROGRAM_H
#include "glad/glad.h"
#include <string>
#include <stdexcept>

namespace PAG {
    class ShaderProgram {
        private:
            GLuint idVS = 0; ///< Identificador del Vertex Shader
            GLuint idFS = 0; ///< Identificador del Fragment Shader
            GLuint idSP = 0; ///< Identificador del Shader Program

            //Funciones auxiliares
            void liberarRecursos();
        public:
            ShaderProgram();
            ~ShaderProgram();

            // Deshabilitar copia para evitar borrados duplicados de recursos OpenGL
            ShaderProgram(const ShaderProgram&) = delete;
            ShaderProgram& operator=(const ShaderProgram&) = delete;

            // Cargar, compilar y enlazar desde archivos .glsl
            void cargarDesdeArchivos(const std::string& nombreBase);

            // Activa el shader program en la GPU (glUseProgram)
            void activar() const;

            // Consulta si el programa está compilado y listo
            bool estaListo() const { return idSP != 0; }

            GLuint getIdProgram() const { return idSP; }
    };
}



#endif //PROYECTO_PAG_SHADERPROGRAM_H
