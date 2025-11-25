#include "colision.h"
#include <cmath>

using namespace std;

// --- Constructor ---
ColisionInelastica::ColisionInelastica(double coefRestitucion)
    : coeficienteRestitucion(coefRestitucion), colisionesTotales(0), energiaPerdida(0.0) {
    if (coefRestitucion < 0.0) coeficienteRestitucion = 0.0;
    if (coefRestitucion > 1.0) coeficienteRestitucion = 1.0;
}

// --- Resolver colisión entre dos partículas ---
void ColisionInelastica::resolverColision(Particula& p1, Particula& p2) {
    if (!p1.estaActiva() || !p2.estaActiva()) return;
    if (!p1.colisionaCon(p2)) return;

    // Obtener datos de las partículas
    Vector r1 = p1.getPosicion();
    Vector r2 = p2.getPosicion();
    Vector v1 = p1.getVelocidad();
    Vector v2 = p2.getVelocidad();
    double m1 = p1.getMasa();
    double m2 = p2.getMasa();

    // Vector normal de colisión (del centro de p1 al centro de p2)
    Vector n = r2 - r1;
    n.normalizar();

    // Velocidades relativas
    Vector vRel = v1 - v2;

    // Velocidad relativa en dirección normal
    double vRelNormal = vRel.dot(n);

    // Si se están alejando, no hay colisión
    if (vRelNormal >= 0) return;

    // Calcular energía antes de la colisión
    double energiaAntes = 0.5 * m1 * v1.magnitud() * v1.magnitud() +
                          0.5 * m2 * v2.magnitud() * v2.magnitud();

    // Impulso de colisión inelástica
    double j = -(1.0 + coeficienteRestitucion) * vRelNormal / (1.0/m1 + 1.0/m2);

    // Aplicar impulso a las velocidades
    Vector impulso = n * j;
    v1 += impulso * (1.0 / m1);
    v2 -= impulso * (1.0 / m2);

    // Actualizar velocidades
    p1.setVelocidad(v1);
    p2.setVelocidad(v2);

    // Calcular energía después de la colisión
    double energiaDespues = 0.5 * m1 * v1.magnitud() * v1.magnitud() +
                            0.5 * m2 * v2.magnitud() * v2.magnitud();

    // Acumular energía perdida
    energiaPerdida += (energiaAntes - energiaDespues);
    colisionesTotales++;

    // Separar partículas para evitar superposición
    separarParticulas(p1, p2);
}

// --- Separar partículas superpuestas ---
void ColisionInelastica::separarParticulas(Particula& p1, Particula& p2) {
    Vector r1 = p1.getPosicion();
    Vector r2 = p2.getPosicion();
    double distancia = p1.distanciaA(p2);
    double sumRadios = p1.getRadio() + p2.getRadio();

    if (distancia < sumRadios) {
        // Vector de separación
        Vector direccion = r2 - r1;
        direccion.normalizar();

        // Cantidad a separar
        double separacion = (sumRadios - distancia) / 2.0;

        // Mover cada partícula en direcciones opuestas
        p1.setPosicion(r1 - direccion * separacion);
        p2.setPosicion(r2 + direccion * separacion);
    }
}

// --- Detectar y resolver todas las colisiones ---
void ColisionInelastica::detectarYResolverColisiones(vector<Particula>& particulas) {
    int n = particulas.size();

    for (int i = 0; i < n; i++) {
        if (!particulas[i].estaActiva()) continue;

        for (int j = i + 1; j < n; j++) {
            if (!particulas[j].estaActiva()) continue;

            if (particulas[i].colisionaCon(particulas[j])) {
                resolverColision(particulas[i], particulas[j]);
            }
        }
    }
}

// --- Calcular energía cinética total ---
double ColisionInelastica::calcularEnergiaCineticaTotal(const vector<Particula>& particulas) const {
    double energiaTotal = 0.0;

    for (const auto& p : particulas) {
        if (p.estaActiva()) {
            Vector v = p.getVelocidad();
            double rapidez = v.magnitud();
            energiaTotal += 0.5 * p.getMasa() * rapidez * rapidez;
        }
    }

    return energiaTotal;
}

// --- Calcular momento lineal total ---
Vector ColisionInelastica::calcularMomentoTotal(const vector<Particula>& particulas) const {
    Vector momentoTotal(0, 0);

    for (const auto& p : particulas) {
        if (p.estaActiva()) {
            momentoTotal += p.getVelocidad() * p.getMasa();
        }
    }

    return momentoTotal;
}

// --- Getters y Setters ---
double ColisionInelastica::getCoeficienteRestitucion() const {
    return coeficienteRestitucion;
}

void ColisionInelastica::setCoeficienteRestitucion(double coef) {
    coeficienteRestitucion = coef;
    if (coeficienteRestitucion < 0.0) coeficienteRestitucion = 0.0;
    if (coeficienteRestitucion > 1.0) coeficienteRestitucion = 1.0;
}

int ColisionInelastica::getColisionesTotales() const {
    return colisionesTotales;
}

double ColisionInelastica::getEnergiaPerdida() const {
    return energiaPerdida;
}

void ColisionInelastica::resetEstadisticas() {
    colisionesTotales = 0;
    energiaPerdida = 0.0;
}

// --- Mostrar estadísticas ---
void ColisionInelastica::mostrarEstadisticas() const {
    cout << "\n=== Estadísticas de Colisiones ===" << endl;
    cout << "Coeficiente de restitución: " << coeficienteRestitucion << endl;
    cout << "Colisiones totales: " << colisionesTotales << endl;
    cout << "Energía perdida acumulada: " << energiaPerdida << " J" << endl;
    cout << "==================================\n" << endl;
}
