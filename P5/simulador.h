#ifndef SIMULADOR_H
#define SIMULADOR_H

#include <vector>
#include <fstream>
#include <string>
#include <map>
#include "particula.h"
#include "obstaculo.h"
#include "colisionManager.h"

class Simulador {
private:
    // --- Contenedores principales ---
    std::vector<Particula*> particulas;
    std::vector<Obstaculo> obstaculos;

    // --- Parámetros de la caja ---
    double ancho;
    double alto;

    // --- Control de tiempo ---
    double tiempoActual;
    double dt;  // Paso de tiempo (e.g., 0.01 segundos)
    double tiempoTotal;

    // --- Registro de datos ---
    std::ofstream archivoPosiciones;
    std::ofstream archivoColisiones;
    std::map<int, std::ofstream*> archivosTrayectorias; // Un archivo por partícula
    int pasoActual;

    // --- Estadísticas ---
    int totalColisionesParticulas;
    int totalColisionesObstaculos;
    int totalColisionesParedes;

    // --- Protección contra bucles ---
    int contadorPasosEstancado;
    int ultimoNumParticulas;
    int siguienteIdParticula;  // Contador secuencial de IDs

public:
    // --- Constructor y Destructor ---
    Simulador(double ancho, double alto, double dt);
    ~Simulador();

    // --- Configuración inicial ---
    void agregarParticula(double x, double y, double vx, double vy,
                          double masa, double radio);
    void agregarObstaculo(double x, double y, double lado, double coefRestitucion);
    void configurarObstaculosDefecto();  // 4 obstáculos predefinidos

    // --- Ejecución de la simulación ---
    void iniciar();
    void ejecutarPaso();
    void ejecutar(double tiempoFinal);
    void finalizar();

    // --- Actualización de física ---
    void actualizarPosiciones();
    void detectarYResolverColisiones();

    // --- Detección de colisiones ---
    void detectarColisionesParedes();
    void detectarColisionesObstaculos();
    void detectarColisionesEntreParticulas();

    // --- Resolución de colisiones ---
    void resolverColisionPared(Particula* p);
    void resolverColisionObstaculo(Particula* p, Obstaculo& obs);
    void resolverColisionEntreParticulas(Particula* p1, Particula* p2);

    // --- Registro y salida ---
    void guardarEstadoActual();
    void registrarColision(const std::string& tipo, int id1, int id2 = -1);
    void abrirArchivos();
    void cerrarArchivos();

    // --- Utilidades ---
    void limpiarParticulasInactivas();
    int contarParticulasActivas() const;
    void mostrarEstadisticas() const;
    bool verificarEstancamiento() const;

    // --- Getters ---
    double getTiempoActual() const { return tiempoActual; }
    int getNumParticulas() const { return particulas.size(); }
    int getNumObstaculos() const { return obstaculos.size(); }
};

#endif // SIMULADOR_H
