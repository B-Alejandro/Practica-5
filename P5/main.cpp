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
    cout << "     SIMULADOR DE COLISIONES DE PARTiCULAS     " << endl;
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

    // --- Parametros de la simulacion ---
    double anchoCaja = 800.0;
    double altoCaja = 600.0;
    double dt = 0.016;   // Paso de tiempo (~60 FPS)
    double tiempoTotal = 50.0;

    cout << "CONFIGURACIoN DE LA SIMULACIoN" << endl;
    cout << "==============================" << endl;
    cout << "Dimensiones de la caja: " << anchoCaja << " x " << altoCaja << endl;
    cout << "Tiempo de simulacion: " << tiempoTotal << " segundos" << endl << endl;

    // --- Tipos de colision segun requisitos ---
    cout << "TIPOS DE COLISIoN:" << endl;
    cout << "- Particulas vs Paredes: ELaSTICAS (conserva energia)" << endl;
    cout << "- Particulas vs Obstaculos: INELaSTICAS (con coef. restitucion)" << endl;
    cout << "- Particulas vs Particulas: COMPLETAMENTE INELaSTICAS (fusion)" << endl << endl;

    // --- Solicitar numero de obstaculos ---
    int numObstaculos;
    cout << "Ingrese el numero de obstaculos (0 - 10): ";
    cin >> numObstaculos;
    numObstaculos = max(0, min(10, numObstaculos));

    // --- Solicitar numero de particulas ---
    int numParticulas;
    cout << "Ingrese el numero de particulas (2 - 20): ";
    cin >> numParticulas;
    numParticulas = max(2, min(20, numParticulas));

    cout << endl;

    // --- Crear simulador con colisiones completamente inelasticas para particulas ---
    // El coeficiente no importa aqui porque las colisiones entre particulas son fusion
    Simulador sim(anchoCaja, altoCaja, dt, TipoColision::COMPLETAMENTE_INELASTICA, 0.0);

    // --- Configurar obstaculos ---
    if (numObstaculos > 0) {
        cout << "Configurando " << numObstaculos << " obstaculos..." << endl;
        sim.configurarObstaculos(numObstaculos);
    } else {
        cout << "No se agregaron obstaculos." << endl;
    }

    // --- Agregar particulas con posiciones y velocidades aleatorias ---
    cout << "Agregando " << numParticulas << " particulas..." << endl;
    srand(static_cast<unsigned>(time(nullptr)));

    for (int i = 0; i < numParticulas; ++i) {
        double x = 50 + rand() % static_cast<int>(anchoCaja - 100);
        double y = 50 + rand() % static_cast<int>(altoCaja - 100);
        double vx = (rand() % 200 - 100);   // -100 a +100 px/s
        double vy = (rand() % 200 - 100);
        double masa = 0.5 + (rand() % 15) / 10.0; // 0.5 a 2.0
        double radio = 10 + rand() % 15;            // 10 a 25 px
        sim.agregarParticula(x, y, vx, vy, masa, radio);
    }

    cout << endl;

    // --- Ejecutar simulacion ---
    sim.iniciar();
    sim.ejecutar(tiempoTotal);
    sim.finalizar();

    // --- Reporte de archivos generados ---
    cout << endl;
    cout << "===============================================" << endl;
    cout << "               ARCHIVOS GENERADOS              " << endl;
    cout << "===============================================" << endl;
    cout << "Trayectorias:" << endl;
    cout << "  - trayectoria_X.txt (para cada particula)" << endl;
    cout << endl;
    cout << "Eventos:" << endl;
    cout << "  - colisiones.txt (registro completo)" << endl;
    cout << endl;
    cout << "Formato: cada linea en trayectorias contiene 'x y'" << endl;
    cout << endl;

    // --- Ejecutar script Python para graficar ---
    cout << "Generando grafica de trayectorias con Python..." << endl;
    int res = system("python ../../graficar_trayectorias.py");

    if (res != 0)
        cout << "No se pudo ejecutar el script de Python." << endl;
    else
        cout << "Grafica generada correctamente." << endl;

    return 0;
}
