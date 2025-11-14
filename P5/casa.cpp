#include "casa.h"

// --- Constructor ---
Casa::Casa() : bloquesDestruidos(0), impactosTotales(0) {}

// --- Construir casa básica (estilo Angry Birds) ---
void Casa::construirCasaBasica(double x, double y) {
    // Base (2 columnas)
    agregarBloque(x - 40, y, 20, 80);      // Columna izquierda
    agregarBloque(x + 40, y, 20, 80);      // Columna derecha

    // Techo horizontal
    agregarBloque(x, y - 50, 100, 20);     // Viga horizontal

    // Techo triangular (3 bloques pequeños)
    agregarBloque(x - 30, y - 70, 40, 15); // Izquierda
    agregarBloque(x + 30, y - 70, 40, 15); // Derecha
    agregarBloque(x, y - 90, 40, 15);      // Punta
}

// --- Construir torre (más alta y estrecha) ---
void Casa::construirCasaTorre(double x, double y) {
    int numPisos = 5;
    double altoBloque = 40;
    double anchoBloque = 30;

    for (int i = 0; i < numPisos; i++) {
        double posY = y - (i * altoBloque);
        agregarBloque(x, posY, anchoBloque, altoBloque);
    }

    // Techo
    agregarBloque(x, y - (numPisos * altoBloque) - 20, 50, 20);
}

// --- Construir pirámide ---
void Casa::construirCasaPiramide(double x, double y) {
    int numNiveles = 4;
    double anchoBase = 40;
    double altoBloque = 30;

    for (int nivel = 0; nivel < numNiveles; nivel++) {
        int bloquesEnNivel = numNiveles - nivel;
        double posY = y - (nivel * altoBloque);

        for (int i = 0; i < bloquesEnNivel; i++) {
            double posX = x - (bloquesEnNivel - 1) * anchoBase / 2.0 + i * anchoBase;
            agregarBloque(posX, posY, anchoBase, altoBloque);
        }
    }
}

// --- Agregar bloque individual ---
void Casa::agregarBloque(double x, double y, double ancho, double alto) {
    int id = bloques.size();
    bloques.push_back(Estructura(id, x, y, ancho, alto));
}

// --- Procesar impacto de partícula ---
void Casa::procesarImpacto(Particula& particula) {
    if (!particula.estaActiva()) return;

    for (auto& bloque : bloques) {
        if (!bloque.estaDestruida() && bloque.colisionaCon(particula)) {
            bloque.recibirImpacto(particula);
            impactosTotales++;

            // Contar bloques destruidos
            if (bloque.estaDestruida()) {
                bloquesDestruidos++;
            }
        }
    }
}

// --- Verificar si toda la casa está destruida ---
bool Casa::estaTotalmenteDestruida() const {
    for (const auto& bloque : bloques) {
        if (!bloque.estaDestruida()) {
            return false;
        }
    }
    return !bloques.empty();
}

// --- Contar bloques activos ---
int Casa::contarBloquesActivos() const {
    int activos = 0;
    for (const auto& bloque : bloques) {
        if (!bloque.estaDestruida()) {
            activos++;
        }
    }
    return activos;
}

// --- Contar bloques destruidos ---
int Casa::contarBloquesDestruidos() const {
    return bloquesDestruidos;
}

// --- Getters ---
const std::vector<Estructura>& Casa::getBloques() const {
    return bloques;
}

int Casa::getImpactosTotales() const {
    return impactosTotales;
}

// --- Reparar toda la casa ---
void Casa::reparar() {
    for (auto& bloque : bloques) {
        bloque.reparar();
    }
    bloquesDestruidos = 0;
    impactosTotales = 0;
}

// --- Limpiar casa (eliminar todos los bloques) ---
void Casa::limpiar() {
    bloques.clear();
    bloquesDestruidos = 0;
    impactosTotales = 0;
}
