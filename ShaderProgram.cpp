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
}
