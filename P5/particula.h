#ifndef PARTICULA_H
#define PARTICULA_H

#include "vector.h"

class Particula {
private:
    int id;                // Identificador único
    Vector posicion;     // Posición (x, y)
    Vector velocidad;    // Velocidad (vx, vy)
    double masa;           // Masa de la partícula
    double radio;          // Radio de la partícula
    bool activa;           // Si está activa en la simulación

public:
    // --- Constructor ---
    Particula(int id, double x, double y, double vx, double vy, double masa, double radio);

    // --- Métodos de movimiento ---
    void mover(double dt);                         // Actualiza la posición
    void colisionarPared(double ancho, double alto); // Rebote elástico con las paredes

    // --- Getters ---
    int getId() const;
    Vector getPosicion() const;
    Vector getVelocidad() const;
    double getMasa() const;
    double getRadio() const;
    bool estaActiva() const;

    // --- Setters ---
    void setPosicion(const Vector2D& p);
    void setVelocidad(const Vector2D& v);
    void setActiva(bool estado);

    // --- Funciones auxiliares ---
    double distanciaA(const Particula& otra) const;  // Distancia entre centros
    bool colisionaCon(const Particula& otra) const;  // Detección de colisión simple
};

#endif // PARTICULA_H
