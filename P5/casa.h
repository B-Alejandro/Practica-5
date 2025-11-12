#ifndef CASA_H
#define CASA_H

#include "estructura.h"
#include "particula.h"
#include <vector>

class Casa {
private:
    std::vector<Estructura> bloques;
    int bloquesDestruidos;
    int impactosTotales;

public:
    // --- Constructor ---
    Casa();

    // --- Construir casa estilo Angry Birds ---
    void construirCasaBasica(double x, double y);
    void construirCasaTorre(double x, double y);
    void construirCasaPiramide(double x, double y);

    // --- Agregar bloque individual ---
    void agregarBloque(double x, double y, double ancho, double alto);

    // --- Procesar impacto de partícula contra todos los bloques ---
    void procesarImpacto(Particula& particula);

    // --- Verificar estado de la casa ---
    bool estaTotalmenteDestruida() const;
    int contarBloquesActivos() const;
    int contarBloquesDestruidos() const;

    // --- Getters ---
    const std::vector<Estructura>& getBloques() const;
    int getImpactosTotales() const;

    // --- Resetear casa ---
    void reparar();
    void limpiar();
};

#endif // CASA_H
