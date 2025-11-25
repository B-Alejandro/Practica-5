#include "colisionmanager.h"
#include <cmath>

// --- Colisión inelástica con obstáculo ---
void ColisionManager::colisionInelastica(Particula& p, Obstaculo& obs) {
    // 1. Determinar qué lado del obstáculo colisionó
    char lado = obs.ladoColision(p.getPosicion());

    // 2. Obtener el vector normal al lado
    Vector normal = obs.getNormal(lado);

    // 3. Obtener velocidad actual
    Vector v = p.getVelocidad();

    // 4. Descomponer en componente normal y paralela
    Vector v_normal = componenteNormal(v, normal);
    Vector v_paralela = componenteParalela(v, normal);

    // 5. Aplicar coeficiente de restitución solo a componente normal
    double epsilon = obs.getCoefRestitucion();
    Vector v_normal_nueva = v_normal * (-epsilon);

    // 6. La componente paralela se mantiene
    Vector v_nueva = v_normal_nueva + v_paralela;

    // 7. Actualizar velocidad
    p.setVelocidad(v_nueva);

    // 8. Corregir posición para evitar solapamiento
    obs.corregirPosicion(p, lado);
}

// --- Componente normal del vector ---
Vector ColisionManager::componenteNormal(const Vector& v, const Vector& normal) {
    // v_⊥ = (v · n̂) * n̂
    double producto = v.dot(normal);
    return normal * producto;
}

// --- Componente paralela del vector ---
Vector ColisionManager::componenteParalela(const Vector& v, const Vector& normal) {
    // v_∥ = v - v_⊥
    return v - componenteNormal(v, normal);
}

// --- Calcular centro de masa ---
Vector ColisionManager::calcularCentroMasa(const Particula& p1, const Particula& p2) {
    double m1 = p1.getMasa();
    double m2 = p2.getMasa();
    Vector pos1 = p1.getPosicion();
    Vector pos2 = p2.getPosicion();

    // Centro de masa: r_cm = (m1*r1 + m2*r2) / (m1 + m2)
    return (pos1 * m1 + pos2 * m2) / (m1 + m2);
}

// --- Calcular nuevo radio ---
double ColisionManager::calcularNuevoRadio(const Particula& p1, const Particula& p2) {
    // Conservación de área (2D): π*R² = π*r1² + π*r2²
    double r1 = p1.getRadio();
    double r2 = p2.getRadio();
    return sqrt(r1 * r1 + r2 * r2);
}

// --- Separar partículas solapadas ---
void ColisionManager::separarParticulas(Particula& p1, Particula& p2) {
    Vector pos1 = p1.getPosicion();
    Vector pos2 = p2.getPosicion();

    Vector diferencia = pos2 - pos1;
    double distancia = diferencia.magnitud();

    if (distancia < EPSILON) {
        diferencia = Vector(1, 0);
        distancia = 1.0;
    }

    Vector direccion = diferencia / distancia;
    double distanciaMinima = p1.getRadio() + p2.getRadio();
    double solapamiento = distanciaMinima - distancia;

    if (solapamiento > 0) {
        double m1 = p1.getMasa();
        double m2 = p2.getMasa();
        double factorM1 = m2 / (m1 + m2);
        double factorM2 = m1 / (m1 + m2);

        Vector correccion1 = direccion * (solapamiento * factorM1);
        Vector correccion2 = direccion * (solapamiento * factorM2);

        p1.setPosicion(pos1 - correccion1);
        p2.setPosicion(pos2 + correccion2);
    }
}

// --- DEPRECATED: Fusión de partículas ---
// Usar ColisionCompletamenteInelastica::fusionarParticulas() en su lugar
Particula* ColisionManager::fusionarParticulas(Particula& p1, Particula& p2, int nuevoId) {
    double m1 = p1.getMasa();
    double m2 = p2.getMasa();
    Vector v1 = p1.getVelocidad();
    Vector v2 = p2.getVelocidad();

    double M = m1 + m2;
    Vector v_nueva = (v1 * m1 + v2 * m2) / M;
    Vector pos_nueva = calcularCentroMasa(p1, p2);
    double radio_nuevo = calcularNuevoRadio(p1, p2);

    Particula* nueva = new Particula(
        nuevoId,
        pos_nueva.getX(),
        pos_nueva.getY(),
        v_nueva.getX(),
        v_nueva.getY(),
        M,
        radio_nuevo
        );

    return nueva;
}
