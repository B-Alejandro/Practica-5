#include "particula.h"
#include <cmath>

// --- Constructor ---
Particula::Particula(int id, double x, double y, double vx, double vy, double masa, double radio)
    : id(id), posicion(x, y), velocidad(vx, vy), masa(masa), radio(radio), activa(true) {}

// --- Movimiento ---
void Particula::mover(double dt) {
    if (!activa) return;
    posicion += velocidad * dt; // Movimiento en tiempo discreto: r = r + v * dt
}

// --- Colisión con las paredes ---
void Particula::colisionarPared(double ancho, double alto) {
    if (!activa) return;

    // Rebote con paredes verticales
    if (posicion.getX() - radio <= 0 || posicion.getX() + radio >= ancho) {
        velocidad.setX(-velocidad.getX());

        // Corrección para no atravesar la pared
        if (posicion.getX() - radio < 0)
            posicion.setX(radio);
        else if (posicion.getX() + radio > ancho)
            posicion.setX(ancho - radio);
    }

    // Rebote con paredes horizontales
    if (posicion.getY() - radio <= 0 || posicion.getY() + radio >= alto) {
        velocidad.setY(-velocidad.getY());

        // Corrección para no atravesar la pared
        if (posicion.getY() - radio < 0)
            posicion.setY(radio);
        else if (posicion.getY() + radio > alto)
            posicion.setY(alto - radio);
    }
}

// --- Getters ---
int Particula::getId() const { return id; }
Vector Particula::getPosicion() const { return posicion; }
Vector Particula::getVelocidad() const { return velocidad; }
double Particula::getMasa() const { return masa; }
double Particula::getRadio() const { return radio; }
bool Particula::estaActiva() const { return activa; }

// --- Setters ---
void Particula::setPosicion(const Vector& p) { posicion = p; }
void Particula::setVelocidad(const Vector& v) { velocidad = v; }
void Particula::setActiva(bool estado) { activa = estado; }

// --- Detección de colisiones entre partículas ---
double Particula::distanciaA(const Particula& otra) const {
    return (posicion - otra.posicion).magnitud();
}

bool Particula::colisionaCon(const Particula& otra) const {
    double distancia = distanciaA(otra);
    return distancia <= (radio + otra.radio);
}
