//
// Created by Juank on 09/10/2026.
//

#include "ShaderProgram.h"

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
}
