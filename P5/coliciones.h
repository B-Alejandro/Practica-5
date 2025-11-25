#ifndef COLISION_H
#define COLISION_H

#include "particula.h"

class ColisionInelastica {
private:
    double coeficienteRestitucion; // e ∈ [0,1]: 0=totalmente inelástica, 1=elástica

public:
    // --- Constructor ---
    ColisionInelastica(double coefRestitucion = 0.8);

    // --- Aplicar colisión inelástica contra superficie horizontal (suelo/techo) ---
    void colisionSuperficieHorizontal(Particula& p);

    // --- Aplicar colisión inelástica contra superficie vertical (pared) ---
    void colisionSuperficieVertical(Particula& p);

    // --- Aplicar colisión inelástica general (automática según velocidad) ---
    void aplicarColision(Particula& p, bool esHorizontal);

    // --- Getters y Setters ---
    double getCoeficienteRestitucion() const;
    void setCoeficienteRestitucion(double coef);

    // --- Calcular velocidad después del rebote ---
    double calcularVelocidadRebote(double velocidadInicial) const;
};

#endif // COLISION_INELASTICA_H
