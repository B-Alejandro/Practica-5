#ifndef OBSTACULO_H
#define OBSTACULO_H

#include "vector.h"
#include "particula.h"

class Obstaculo {
private:
    Vector posicion;           // Esquina superior izquierda
    double lado;               // Tamaño del cuadrado
    double coefRestitucion;    // Coeficiente ε (0 < ε < 1)

public:
    // --- Constructor ---
    Obstaculo(double x, double y, double lado, double coefRestitucion);

    // --- Detección de colisión ---
    bool colisionaCon(const Particula& p) const;

    // --- Determinar lado del cuadrado que colisionó ---
    // Devuelve: 'T' (top), 'B' (bottom), 'L' (left), 'R' (right)
    char ladoColision(const Vector& posParticula) const;

    // --- Obtener vector normal al lado ---
    Vector getNormal(char lado) const;

    // --- Corregir posición de partícula para evitar solapamiento ---
    void corregirPosicion(Particula& p, char lado) const;

    // --- Getters ---
    Vector getPosicion() const { return posicion; }
    double getLado() const { return lado; }
    double getCoefRestitucion() const { return coefRestitucion; }

    // --- Límites del obstáculo ---
    double getLeft() const { return posicion.getX(); }
    double getRight() const { return posicion.getX() + lado; }
    double getTop() const { return posicion.getY(); }
    double getBottom() const { return posicion.getY() + lado; }

    // --- Centro del obstáculo ---
    Vector getCentro() const;
};

#endif // OBSTACULO_H
