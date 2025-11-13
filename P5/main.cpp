#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <ctime>
#include "simulador.h"

using namespace std;
namespace fs = std::filesystem;

int main() {
    cout << "===============================================" << endl;
    cout << "     SIMULADOR DE COLISIONES - CONFIGURABLE     " << endl;
    cout << "===============================================" << endl << endl;

    // --- Limpiar archivos previos ---
    cout << "Eliminando archivos de ejecuciones anteriores..." << endl;
    try {
        for (const auto& entry : fs::directory_iterator(".")) {
            string name = entry.path().filename().string();
            if (name.rfind("trayectoria_", 0) == 0 && name.ends_with(".txt")) {
                fs::remove(entry.path());
            }
            if (name == "colisiones.txt") {
                fs::remove(entry.path());
            }
        }
        cout << "Archivos anteriores eliminados correctamente.\n" << endl;
    } catch (const exception& e) {
        cerr << "Error al limpiar archivos previos: " << e.what() << endl;
    }

    // --- Parámetros base ---
    double anchoCaja = 800.0;
    double altoCaja = 600.0;
    double dt = 0.016;   // Paso de tiempo (~60 FPS)
    double tiempoTotal = 10.0;

    // --- Seleccionar tipo de colisión ---
    cout << "Seleccione el tipo de colision entre particulas:" << endl;
    cout << "1. Elasticas (e = 1.0) - Conserva energia" << endl;
    cout << "2. Inelasticas (0 < e < 1) - Pierde energia" << endl;
    cout << "3. Completamente inelasticas (e = 0) - Fusion" << endl;
    cout << "Opcion (1-3): ";

    int opcion;
    cin >> opcion;

    TipoColision tipoColision;
    double coefRestitucion = 0.8;

    switch (opcion) {
    case 1:
        tipoColision = TipoColision::ELASTICA;
        cout << "\nModo: COLISIONES ELASTICAS" << endl;
        break;
    case 2:
        tipoColision = TipoColision::INELASTICA;
        cout << "Ingrese coeficiente de restitucion (0.0 - 1.0): ";
        cin >> coefRestitucion;
        coefRestitucion = max(0.0, min(1.0, coefRestitucion));
        cout << "\nModo: COLISIONES INELASTICAS (e=" << coefRestitucion << ")" << endl;
        break;
    case 3:
    default:
        tipoColision = TipoColision::COMPLETAMENTE_INELASTICA;
        cout << "\nModo: FUSION DE PARTICULAS" << endl;
        break;
    }

    // --- Crear simulador ---
    Simulador sim(anchoCaja, altoCaja, dt, tipoColision, coefRestitucion);

    // --- Elegir número de obstáculos ---
    int numObstaculos = 4;
    cout << "\nIngrese el numero de obstaculos (0 - 10): ";
    cin >> numObstaculos;
    numObstaculos = max(0, min(10, numObstaculos));

    if (numObstaculos > 0)
        sim.configurarObstaculos(numObstaculos);
    else
        cout << "No se agregaron obstaculos." << endl;

    // --- Elegir número de partículas ---
    int numParticulas;
    cout << "\nIngrese el numero de particulas (1 - 20): ";
    cin >> numParticulas;
    numParticulas = max(1, min(20, numParticulas));

    cout << "\nAgregando particulas iniciales..." << endl;

    // --- Generar partículas distribuidas aleatoriamente ---
    srand(static_cast<unsigned>(time(nullptr)));
    for (int i = 0; i < numParticulas; ++i) {
        double x = 50 + rand() % static_cast<int>(anchoCaja - 100);
        double y = 50 + rand() % static_cast<int>(altoCaja - 100);
        double vx = (rand() % 200 - 100);   // -100 a +100 px/s
        double vy = (rand() % 200 - 100);
        double masa = 0.5 + (rand() % 150) / 100.0; // 0.5 a 2.0
        double radio = 10 + rand() % 15;            // 10 a 25 px
        sim.agregarParticula(x, y, vx, vy, masa, radio);
    }

    // --- Ejecutar simulación ---
    sim.iniciar();
    sim.ejecutar(tiempoTotal);
    sim.finalizar();

    // --- Reporte de salida ---
    cout << endl;
    cout << "===============================================" << endl;
    cout << "               ARCHIVOS GENERADOS              " << endl;
    cout << "===============================================" << endl;
    for (int i = 0; i < numParticulas; ++i) {
        cout << "  - trayectoria_" << i << ".txt (particula " << i << ")" << endl;
    }

    if (tipoColision == TipoColision::COMPLETAMENTE_INELASTICA) {
        cout << "  - trayectoria fusionadas (si ocurren)" << endl;
    }

    cout << endl << "Eventos:" << endl;
    cout << "  - colisiones.txt (registro completo)" << endl;
    cout << endl << "Formato: cada linea en trayectorias contiene 'x y'" << endl;

    // --- Ejecutar script Python ---
    cout << endl << "Generando grafica de trayectorias con Python..." << endl;
    int res = system("python3 ../../graficar_trayectorias.py");

    if (res != 0)
        cout << "No se pudo ejecutar el script de Python." << endl;
    else
        cout << "Grafica generada correctamente." << endl;

    return 0;
}
