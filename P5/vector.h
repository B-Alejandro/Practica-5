#ifndef VECTOR_H
#define VECTOR_H

#include <cmath>
#include <iostream>

class Vector {
private:
    double x;
    double y;

public:
    // --- Constructores ---
    Vector();                       // Vector (0,0)
    Vector(double x, double y);     // Vector (x, y)

    // --- Getters y setters ---
    double getX() const;
    double getY() const;
    void setX(double nx);
    void setY(double ny);

    // --- Operaciones básicas ---
    double magnitud() const;            // |v|
    void normalizar();                  // v -> v/|v|
    double angulo() const;              // ángulo en radianes

    // --- Operadores sobrecargados ---
    Vector operator+(const Vector& v) const;
    Vector operator-(const Vector& v) const;
    Vector operator*(double escalar) const;
    Vector operator/(double escalar) const;
    Vector& operator+=(const Vector& v);
    Vector& operator-=(const Vector& v);
    bool operator==(const Vector& v) const;

    // --- Producto punto ---
    double dot(const Vector& v) const;

    // --- Mostrar vector ---
    void mostrar() const;
};

#endif // VECTOR_H
