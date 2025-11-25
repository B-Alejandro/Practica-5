#include "vector.h"
#include <iostream>
#include <cmath>

using namespace std;

// --- Constructores ---
Vector::Vector() : x(0), y(0) {}
Vector::Vector(double x, double y) : x(x), y(y) {}

// --- Getters y setters ---
double Vector::getX() const { return x; }
double Vector::getY() const { return y; }
void Vector::setX(double nx) { x = nx; }
void Vector::setY(double ny) { y = ny; }

// --- Magnitud, normalizar y ángulo ---
double Vector::magnitud() const {
    return sqrt(x * x + y * y);
}

void Vector::normalizar() {
    double m = magnitud();
    if (m != 0) {
        x /= m;
        y /= m;
    }
}

double Vector::angulo() const {
    return atan2(y, x);
}

// --- Operadores ---
Vector Vector::operator+(const Vector& v) const {
    return Vector(x + v.x, y + v.y);
}

Vector Vector::operator-(const Vector& v) const {
    return Vector(x - v.x, y - v.y);
}

Vector Vector::operator*(double escalar) const {
    return Vector(x * escalar, y * escalar);
}

Vector Vector::operator/(double escalar) const {
    return Vector(x / escalar, y / escalar);
}

Vector& Vector::operator+=(const Vector& v) {
    x += v.x;
    y += v.y;
    return *this;
}

Vector& Vector::operator-=(const Vector& v) {
    x -= v.x;
    y -= v.y;
    return *this;
}

bool Vector::operator==(const Vector& v) const {
    return (x == v.x && y == v.y);
}

double Vector::dot(const Vector& v) const {
    return x * v.x + y * v.y;
}

void Vector::mostrar() const {
    cout << "(" << x << ", " << y << ")";
}
