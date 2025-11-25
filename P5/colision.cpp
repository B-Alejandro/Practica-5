#include "colision.h"
#include <cmath>
#include <iostream>

using namespace std;

// ============================================================================
// CLASE BASE: Colision
// ============================================================================

Colision::Colision() : colisionesTotales(0), energiaPerdida(0.0) {}

void Colision::separarParticulas(Particula& p1, Particula& p2) {
    Vector r1 = p1.getPosicion();
    Vector r2 = p2.getPosicion();
    double distancia = p1.distanciaA(p2);
    double sumRadios = p1.getRadio() + p2.getRadio();

    if (distancia < sumRadios) {
        Vector direccion = r2 - r1;

        if (distancia < EPSILON) {
            direccion = Vector(1, 0);
            distancia = 1.0;
        }

        direccion.normalizar();
        double separacion = (sumRadios - distancia) / 2.0;

        // Separar proporcionalmente a las masas inversas
        double m1 = p1.getMasa();
        double m2 = p2.getMasa();
        double factorM1 = m2 / (m1 + m2);
        double factorM2 = m1 / (m1 + m2);

        p1.setPosicion(r1 - direccion * (separacion * factorM1));
        p2.setPosicion(r2 + direccion * (separacion * factorM2));
    }
}

void Colision::detectarYResolverColisiones(vector<Particula*>& particulas) {
    int n = particulas.size();

    for (int i = 0; i < n; i++) {
        if (!particulas[i]->estaActiva()) continue;

        for (int j = i + 1; j < n; j++) {
            if (!particulas[j]->estaActiva()) continue;

            if (particulas[i]->colisionaCon(*particulas[j])) {
                resolverColision(*particulas[i], *particulas[j]);
            }
        }
    }
}

double Colision::calcularEnergiaCineticaTotal(const vector<Particula*>& particulas) const {
    double energiaTotal = 0.0;

    for (const auto* p : particulas) {
        if (p->estaActiva()) {
            Vector v = p->getVelocidad();
            double rapidez = v.magnitud();
            energiaTotal += 0.5 * p->getMasa() * rapidez * rapidez;
        }
    }

    return energiaTotal;
}

Vector Colision::calcularMomentoTotal(const vector<Particula*>& particulas) const {
    Vector momentoTotal(0, 0);

    for (const auto* p : particulas) {
        if (p->estaActiva()) {
            momentoTotal += p->getVelocidad() * p->getMasa();
        }
    }

    return momentoTotal;
}

int Colision::getColisionesTotales() const {
    return colisionesTotales;
}

double Colision::getEnergiaPerdida() const {
    return energiaPerdida;
}

void Colision::resetEstadisticas() {
    colisionesTotales = 0;
    energiaPerdida = 0.0;
}

void Colision::mostrarEstadisticas() const {
    cout << "Colisiones totales: " << colisionesTotales << endl;
    cout << "Energía perdida: " << energiaPerdida << " J" << endl;
}

// ============================================================================
// COLISION ELASTICA (e = 1.0)
// ============================================================================

ColisionElastica::ColisionElastica() : Colision() {}

void ColisionElastica::resolverColision(Particula& p1, Particula& p2) {
    if (!p1.estaActiva() || !p2.estaActiva()) return;
    if (!p1.colisionaCon(p2)) return;

    Vector r1 = p1.getPosicion();
    Vector r2 = p2.getPosicion();
    Vector v1 = p1.getVelocidad();
    Vector v2 = p2.getVelocidad();
    double m1 = p1.getMasa();
    double m2 = p2.getMasa();

    // Vector normal de colisión
    Vector n = r2 - r1;
    n.normalizar();

    // Velocidad relativa
    Vector vRel = v1 - v2;
    double vRelNormal = vRel.dot(n);

    // Si se están alejando, no hay colisión
    if (vRelNormal >= 0) return;

    // Impulso para colisión elástica (e = 1)
    double j = -2.0 * vRelNormal / (1.0/m1 + 1.0/m2);

    Vector impulso = n * j;
    v1 += impulso * (1.0 / m1);
    v2 -= impulso * (1.0 / m2);

    p1.setVelocidad(v1);
    p2.setVelocidad(v2);

    colisionesTotales++;
    separarParticulas(p1, p2);
}

void ColisionElastica::mostrarEstadisticas() const {
    cout << "\n=== Colisiones Elásticas ===" << endl;
    cout << "Coeficiente de restitución: 1.0" << endl;
    Colision::mostrarEstadisticas();
    cout << "Energía conservada (teóricamente)" << endl;
    cout << "==============================\n" << endl;
}

// ============================================================================
// COLISION INELASTICA (0 < e < 1)
// ============================================================================

ColisionInelastica::ColisionInelastica(double coefRestitucion)
    : Colision(), coeficienteRestitucion(coefRestitucion) {
    if (coefRestitucion < 0.0) coeficienteRestitucion = 0.0;
    if (coefRestitucion > 1.0) coeficienteRestitucion = 1.0;
}

void ColisionInelastica::resolverColision(Particula& p1, Particula& p2) {
    if (!p1.estaActiva() || !p2.estaActiva()) return;
    if (!p1.colisionaCon(p2)) return;

    Vector r1 = p1.getPosicion();
    Vector r2 = p2.getPosicion();
    Vector v1 = p1.getVelocidad();
    Vector v2 = p2.getVelocidad();
    double m1 = p1.getMasa();
    double m2 = p2.getMasa();

    // Vector normal de colisión
    Vector n = r2 - r1;
    n.normalizar();

    // Velocidad relativa
    Vector vRel = v1 - v2;
    double vRelNormal = vRel.dot(n);

    // Si se están alejando, no hay colisión
    if (vRelNormal >= 0) return;

    // Calcular energía antes
    double energiaAntes = 0.5 * m1 * v1.magnitud() * v1.magnitud() +
                          0.5 * m2 * v2.magnitud() * v2.magnitud();

    // Impulso de colisión inelástica
    double j = -(1.0 + coeficienteRestitucion) * vRelNormal / (1.0/m1 + 1.0/m2);

    Vector impulso = n * j;
    v1 += impulso * (1.0 / m1);
    v2 -= impulso * (1.0 / m2);

    p1.setVelocidad(v1);
    p2.setVelocidad(v2);

    // Calcular energía después
    double energiaDespues = 0.5 * m1 * v1.magnitud() * v1.magnitud() +
                            0.5 * m2 * v2.magnitud() * v2.magnitud();

    energiaPerdida += (energiaAntes - energiaDespues);
    colisionesTotales++;

    separarParticulas(p1, p2);
}

void ColisionInelastica::mostrarEstadisticas() const {
    cout << "\n=== Colisiones Inelásticas ===" << endl;
    cout << "Coeficiente de restitución: " << coeficienteRestitucion << endl;
    Colision::mostrarEstadisticas();
    cout << "==============================\n" << endl;
}

double ColisionInelastica::getCoeficienteRestitucion() const {
    return coeficienteRestitucion;
}

void ColisionInelastica::setCoeficienteRestitucion(double coef) {
    coeficienteRestitucion = coef;
    if (coeficienteRestitucion < 0.0) coeficienteRestitucion = 0.0;
    if (coeficienteRestitucion > 1.0) coeficienteRestitucion = 1.0;
}

// ============================================================================
// COLISION COMPLETAMENTE INELASTICA (e = 0) - FUSION
// ============================================================================

ColisionCompletamenteInelastica::ColisionCompletamenteInelastica(int idInicial)
    : Colision(), siguienteId(idInicial) {}

void ColisionCompletamenteInelastica::resolverColision(Particula& p1, Particula& p2) {
    // Esta versión solo marca las partículas como inactivas
    // La fusión real se hace con fusionarParticulas()
    if (!p1.estaActiva() || !p2.estaActiva()) return;
    if (!p1.colisionaCon(p2)) return;

    separarParticulas(p1, p2);
    colisionesTotales++;
}

Particula* ColisionCompletamenteInelastica::fusionarParticulas(Particula& p1, Particula& p2) {
    double m1 = p1.getMasa();
    double m2 = p2.getMasa();
    Vector v1 = p1.getVelocidad();
    Vector v2 = p2.getVelocidad();

    // Calcular energía antes
    double energiaAntes = 0.5 * m1 * v1.magnitud() * v1.magnitud() +
                          0.5 * m2 * v2.magnitud() * v2.magnitud();

    // Nueva masa
    double M = m1 + m2;

    // Conservación del momento lineal
    Vector v_nueva = (v1 * m1 + v2 * m2) / M;

    // Posición del centro de masa
    Vector pos_nueva = calcularCentroMasa(p1, p2);

    // Nuevo radio (conservación de área)
    double radio_nuevo = calcularNuevoRadio(p1, p2);

    // Crear nueva partícula
    Particula* nueva = new Particula(
        siguienteId++,
        pos_nueva.getX(),
        pos_nueva.getY(),
        v_nueva.getX(),
        v_nueva.getY(),
        M,
        radio_nuevo
        );

    // Calcular energía después
    double energiaDespues = 0.5 * M * v_nueva.magnitud() * v_nueva.magnitud();
    energiaPerdida += (energiaAntes - energiaDespues);

    return nueva;
}

void ColisionCompletamenteInelastica::mostrarEstadisticas() const {
    cout << "\n=== Colisiones Completamente Inelásticas (Fusión) ===" << endl;
    cout << "Coeficiente de restitución: 0.0" << endl;
    Colision::mostrarEstadisticas();
    cout << "================================================\n" << endl;
}

void ColisionCompletamenteInelastica::setSiguienteId(int id) {
    siguienteId = id;
}

Vector ColisionCompletamenteInelastica::calcularCentroMasa(const Particula& p1, const Particula& p2) const {
    double m1 = p1.getMasa();
    double m2 = p2.getMasa();
    Vector pos1 = p1.getPosicion();
    Vector pos2 = p2.getPosicion();

    return (pos1 * m1 + pos2 * m2) / (m1 + m2);
}

double ColisionCompletamenteInelastica::calcularNuevoRadio(const Particula& p1, const Particula& p2) const {
    // Conservación de área (2D): π*R² = π*r1² + π*r2²
    double r1 = p1.getRadio();
    double r2 = p2.getRadio();
    return sqrt(r1 * r1 + r2 * r2);
}
