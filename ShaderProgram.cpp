//
// Created by Juank on 09/10/2026.
//

#include "ShaderProgram.h"

#include <fstream>
#include <sstream>

namespace PAG {
    ShaderProgram::ShaderProgram() = default;

    ShaderProgram::~ShaderProgram() {
        liberarRecursos();
    }

    void ShaderProgram::liberarRecursos() {
        if (idVS != 0) { glDeleteShader(idVS); idVS = 0; }
        if (idFS != 0) { glDeleteShader(idFS); idFS = 0; }
        if (idSP != 0) { glDeleteProgram(idSP); idSP = 0; }
    }

    std::string ShaderProgram::cargarArchivoTexto(const std::string& rutaArchivo) {
        std::ifstream archivo(rutaArchivo);
        if (!archivo.is_open()) {
            throw std::runtime_error("No se pudo abrir el archivo de shader: " + rutaArchivo);
        }
        std::stringstream buffer;
        buffer << archivo.rdbuf();
        return buffer.str();
    }

    void ShaderProgram::comprobarCompilacionShader(GLuint shader, const std::string& tipoShader) {
        GLint compilado = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compilado);

        if (compilado == GL_FALSE) {
            GLint logLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

            std::string logMsg(logLength, '\0');
            glGetShaderInfoLog(shader, logLength, &logLength, &logMsg[0]);

            throw std::runtime_error("Error al compilar " + tipoShader + ":\n" + logMsg);
        }
    }

    void ShaderProgram::comprobarEnlazadoProgram(GLuint program) {
        GLint enlazado = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &enlazado);

        if (enlazado == GL_FALSE) {
            GLint logLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

            std::string logMsg(logLength, '\0');
            glGetProgramInfoLog(program, logLength, &logLength, &logMsg[0]);

            throw std::runtime_error("Error al enlazar el Shader Program:\n" + logMsg);
        }
    }
}
