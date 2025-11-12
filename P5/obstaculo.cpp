#include "obstaculo.h"
#include <cmath>
#include <algorithm>

using namespace std;

// --- Constructor ---
Obstaculo::Obstaculo(double x, double y, double lado, double coefRestitucion)
    : posicion(x, y), lado(lado), coefRestitucion(coefRestitucion) {}

// --- Detectar colisión con partícula ---
bool Obstaculo::colisionaCon(const Particula& p) const {
    Vector posP = p.getPosicion();
    double radio = p.getRadio();

    // Encontrar el punto más cercano del cuadrado a la partícula
    double puntoX = max(getLeft(), min(posP.getX(), getRight()));
    double puntoY = max(getTop(), min(posP.getY(), getBottom()));

    // Calcular distancia del centro de la partícula al punto más cercano
    double dx = posP.getX() - puntoX;
    double dy = posP.getY() - puntoY;
    double distancia = sqrt(dx * dx + dy * dy);

    // Hay colisión si la distancia es menor que el radio
    return distancia < radio;
}

// --- Determinar qué lado colisionó ---
char Obstaculo::ladoColision(const Vector& posParticula) const {
    // Calcular distancias a cada lado
    double distLeft = abs(posParticula.getX() - getLeft());
    double distRight = abs(posParticula.getX() - getRight());
    double distTop = abs(posParticula.getY() - getTop());
    double distBottom = abs(posParticula.getY() - getBottom());

    // Encontrar la distancia mínima
    double minDist = min({distLeft, distRight, distTop, distBottom});

    // Devolver el lado correspondiente
    if (minDist == distLeft) return 'L';
    if (minDist == distRight) return 'R';
    if (minDist == distTop) return 'T';
    return 'B';  // distBottom
}

// --- Obtener vector normal al lado ---
Vector Obstaculo::getNormal(char lado) const {
    switch(lado) {
    case 'T': return Vector(0, -1);  // Normal apunta hacia arriba
    case 'B': return Vector(0, 1);   // Normal apunta hacia abajo
    case 'L': return Vector(-1, 0);  // Normal apunta a la izquierda
    case 'R': return Vector(1, 0);   // Normal apunta a la derecha
    default: return Vector(0, 1);
    }
}

// --- Corregir posición para evitar solapamiento ---
void Obstaculo::corregirPosicion(Particula& p, char lado) const {
    Vector pos = p.getPosicion();
    double radio = p.getRadio();

    switch(lado) {
    case 'T':  // Colisión arriba
        if (pos.getY() - radio < getTop()) {
            pos.setY(getTop() - radio);
        }
        break;
    case 'B':  // Colisión abajo
        if (pos.getY() + radio > getBottom()) {
            pos.setY(getBottom() + radio);
        }
        break;
    case 'L':  // Colisión izquierda
        if (pos.getX() - radio < getLeft()) {
            pos.setX(getLeft() - radio);
        }
        break;
    case 'R':  // Colisión derecha
        if (pos.getX() + radio > getRight()) {
            pos.setX(getRight() + radio);
        }
        break;
    }

    p.setPosicion(pos);
}

// --- Obtener centro del obstáculo ---
Vector Obstaculo::getCentro() const {
    return Vector(posicion.getX() + lado / 2.0,
                  posicion.getY() + lado / 2.0);
}
