#ifndef COLISION_MANAGER_H
#define COLISION_MANAGER_H

#include "particula.h"
#include "obstaculo.h"
#include "vector.h"
#include "colision.h"

/**
 * @brief Gestor de colisiones que maneja todos los tipos de interacciones
 * Integra colisiones con obstáculos y permite seleccionar el tipo de
 * colisión entre partículas mediante herencia polimórfica
 */
class ColisionManager {
public:
    // --- Colisión con obstáculo (inelástica con coeficiente ε) ---
    static void colisionInelastica(Particula& p, Obstaculo& obs);

    // --- Utilidades para descomposición de vectores ---
    static Vector componenteNormal(const Vector& v, const Vector& normal);
    static Vector componenteParalela(const Vector& v, const Vector& normal);

    // --- Separar partículas solapadas ---
    static void separarParticulas(Particula& p1, Particula& p2);

    // --- Métodos auxiliares para fusión (usados por ColisionCompletamenteInelastica) ---
    static Vector calcularCentroMasa(const Particula& p1, const Particula& p2);
    static double calcularNuevoRadio(const Particula& p1, const Particula& p2);

    // DEPRECATED: Usar ColisionCompletamenteInelastica::fusionarParticulas() en su lugar
    static Particula* fusionarParticulas(Particula& p1, Particula& p2, int nuevoId);

private:
    static constexpr double EPSILON = 1e-10;
};

#endif // COLISION_MANAGER_H
