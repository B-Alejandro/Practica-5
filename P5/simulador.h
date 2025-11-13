#ifndef SIMULADOR_H
#define SIMULADOR_H

#include <vector>
#include <map>
#include <string>
#include <fstream>
#include "particula.h"
#include "obstaculo.h"
#include "colision.h"
#include "colisionmanager.h"

enum class TipoColision {
    ELASTICA,
    INELASTICA,
    COMPLETAMENTE_INELASTICA
};

/**
 * @brief Clase principal que gestiona toda la simulación de partículas.
 * Controla las partículas, obstáculos, detección y resolución de colisiones,
 * archivos de salida y estadísticas generales.
 */
class Simulador {
private:
    // --- Parámetros de la simulación ---
    double ancho;
    double alto;
    double dt;
    double tiempoActual;
    double tiempoTotal;
    int pasoActual;

    // --- Estadísticas ---
    int totalColisionesParticulas;
    int totalColisionesObstaculos;
    int totalColisionesParedes;

    // --- Control de estado ---
    int contadorPasosEstancado;
    int ultimoNumParticulas;
    int siguienteIdParticula;

    // --- Entidades ---
    std::vector<Particula*> particulas;
    std::vector<Obstaculo> obstaculos;

    // --- Sistema de colisiones ---
    Colision* motorColisiones;
    TipoColision tipoColisionActual;

    // --- Archivos ---
    std::ofstream archivoColisiones;
    std::map<int, std::ofstream*> archivosTrayectorias;

public:
    // --- Constructores / Destructores ---
    Simulador(double ancho, double alto, double dt, TipoColision tipo, double coefRestitucion);
    ~Simulador();

    // --- Configuración de tipo de colisión ---
    void setTipoColision(TipoColision tipo, double coefRestitucion);
    TipoColision getTipoColision() const;

    // --- Gestión de entidades ---
    void agregarParticula(double x, double y, double vx, double vy, double masa, double radio);
    void agregarObstaculo(double x, double y, double lado, double coefRestitucion);
    void configurarObstaculos(int cantidad);

    // --- Ciclo de simulación ---
    void iniciar();
    void ejecutar(double tiempoFinal);
    void ejecutarPaso();
    void finalizar();

private:
    // --- Lógica interna ---
    void actualizarPosiciones();
    void detectarYResolverColisiones();
    void detectarColisionesParedes();
    void detectarColisionesObstaculos();
    void detectarColisionesEntreParticulas();

    void resolverColisionPared(Particula* p);
    void resolverColisionObstaculo(Particula* p, Obstaculo& obs);
    void resolverColisionEntreParticulas(Particula* p1, Particula* p2);

    // --- Archivos y registro ---
    void abrirArchivos();
    void cerrarArchivos();
    void guardarEstadoActual();
    void registrarColision(const std::string& tipo, int id1, int id2 = -1);

    // --- Utilidades ---
    void limpiarParticulasInactivas();
    int contarParticulasActivas() const;
    bool verificarEstancamiento() const;
    void mostrarEstadisticas() const;
};

#endif // SIMULADOR_H
