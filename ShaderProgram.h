//
// Created by Juank on 09/10/2026.
//

#ifndef PROYECTO_PAG_SHADERPROGRAM_H
#define PROYECTO_PAG_SHADERPROGRAM_H

namespace PAG {
    class ShaderProgram {
        private:
            GLuint idVS = 0; ///< Identificador del Vertex Shader
            GLuint idFS = 0; ///< Identificador del Fragment Shader
            GLuint idSP = 0; ///< Identificador del Shader Program

        public:
            ShaderProgram();
            ~ShaderProgram();
    };
}



#endif //PROYECTO_PAG_SHADERPROGRAM_H
