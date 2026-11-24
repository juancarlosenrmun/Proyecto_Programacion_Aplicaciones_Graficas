#ifndef PROYECTO_PAG_LUZ_H
#define PROYECTO_PAG_LUZ_H

#include <glm/glm.hpp>
#include "ShaderProgram.h"

namespace PAG {

    enum class TipoLuz {
        AMBIENTE = 0,
        PUNTUAL = 1,
        DIRECCIONAL = 2,
        FOCO = 3,
        NUM_TIPOS = 4
    };

    struct PropiedadesLuz {
        bool activa = true;
        glm::vec3 Ia = glm::vec3(0.2f, 0.2f, 0.2f);
        glm::vec3 Id = glm::vec3(0.8f, 0.8f, 0.8f);
        glm::vec3 Is = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 p  = glm::vec3(2.0f, 3.0f, 2.0f);    // Posición (mundo)
        glm::vec3 d  = glm::vec3(-1.0f, -1.0f, -1.0f); // Dirección (desde la luz hacia la escena)
        float gamma  = 25.0f;                          // Ángulo de apertura del foco (grados)
        float s      = 8.0f;                           // Exponente de bordes suaves
    };

    class AplicadorLuz {
    public:
        virtual ~AplicadorLuz() = default;
        virtual void aplicarLuz(const PropiedadesLuz& p, const glm::mat4& mVis, const ShaderProgram& sp) const = 0;
    };

    class AplicadorLuzAmbiente : public AplicadorLuz {
    public:
        void aplicarLuz(const PropiedadesLuz& p, const glm::mat4& mVis, const ShaderProgram& sp) const override;
    };

    class AplicadorLuzPuntual : public AplicadorLuz {
    public:
        void aplicarLuz(const PropiedadesLuz& p, const glm::mat4& mVis, const ShaderProgram& sp) const override;
    };

    class AplicadorLuzDireccional : public AplicadorLuz {
    public:
        void aplicarLuz(const PropiedadesLuz& p, const glm::mat4& mVis, const ShaderProgram& sp) const override;
    };

    class AplicadorLuzFoco : public AplicadorLuz {
    public:
        void aplicarLuz(const PropiedadesLuz& p, const glm::mat4& mVis, const ShaderProgram& sp) const override;
    };

    class Luz {
        private:
            TipoLuz tipo;
            PropiedadesLuz propiedades;
    };
}


#endif //PROYECTO_PAG_LUZ_H
