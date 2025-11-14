#include "simulador.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <cmath>

using namespace std;

Simulador::Simulador(double ancho, double alto, double dt, TipoColision tipo, double coefRestitucion)
    : ancho(ancho), alto(alto), dt(dt), tiempoActual(0.0),
    tiempoTotal(0.0), pasoActual(0),
    totalColisionesParticulas(0), totalColisionesObstaculos(0),
    totalColisionesParedes(0), contadorPasosEstancado(0),
    ultimoNumParticulas(0), siguienteIdParticula(0),
    motorColisiones(nullptr), tipoColisionActual(tipo) {

    // Siempre usar fusión para partículas
    motorColisiones = new ColisionCompletamenteInelastica(siguienteIdParticula);
}

Simulador::~Simulador() {
    for (Particula* p : particulas) {
        delete p;
    }
    particulas.clear();

    if (motorColisiones) {
        delete motorColisiones;
    }

    cerrarArchivos();
}

void Simulador::agregarParticula(double x, double y, double vx, double vy,
                                 double masa, double radio) {
    Particula* nueva = new Particula(siguienteIdParticula, x, y, vx, vy, masa, radio);
    particulas.push_back(nueva);
    siguienteIdParticula++;

    auto* motorFusion = dynamic_cast<ColisionCompletamenteInelastica*>(motorColisiones);
    if (motorFusion) {
        motorFusion->setSiguienteId(siguienteIdParticula);
    }
}

void Simulador::agregarObstaculo(double x, double y, double lado, double coefRestitucion) {
    obstaculos.emplace_back(x, y, lado, coefRestitucion);
}

void Simulador::configurarObstaculos(int cantidad) {
    // Coeficiente de restitución para obstáculos (inelástica)
    double coef = 0.7;
    double lado = 50.0;

    for (int i = 0; i < cantidad; ++i) {
        double x = (ancho / (cantidad + 1)) * (i + 1) - lado / 2.0;
        double y = (alto / (cantidad + 1)) * (i + 1) - lado / 2.0;
        agregarObstaculo(x, y, lado, coef);
    }
}

void Simulador::iniciar() {
    tiempoActual = 0.0;
    pasoActual = 0;
    abrirArchivos();

    cout << "===============================================" << endl;
    cout << "             INICIO DE SIMULACIÓN              " << endl;
    cout << "===============================================" << endl;
    cout << "Partículas iniciales: " << particulas.size() << endl;
    cout << "Obstáculos: " << obstaculos.size() << endl;
    cout << "Dimensiones: " << ancho << " x " << alto << endl;
    cout << "dt: " << dt << " s" << endl;
    cout << endl;
    cout << "TIPOS DE COLISIÓN CONFIGURADOS:" << endl;
    cout << "  • Partículas ↔ Paredes: ELÁSTICA (e=1.0)" << endl;
    cout << "  • Partículas ↔ Obstáculos: INELÁSTICA (e=0.7)" << endl;
    cout << "  • Partículas ↔ Partículas: FUSIÓN (e=0.0)" << endl;
    cout << endl;
}

void Simulador::ejecutarPaso() {
    actualizarPosiciones();
    detectarYResolverColisiones();
    guardarEstadoActual();
    limpiarParticulasInactivas();

    int activasAhora = contarParticulasActivas();
    if (activasAhora == ultimoNumParticulas) {
        contadorPasosEstancado++;
    } else {
        contadorPasosEstancado = 0;
        ultimoNumParticulas = activasAhora;
    }

    tiempoActual += dt;
    pasoActual++;
}

void Simulador::ejecutar(double tiempoFinal) {
    tiempoTotal = tiempoFinal;
    int totalPasos = static_cast<int>(tiempoFinal / dt);
    int progresoAnterior = -1;

    for (int i = 0; i < totalPasos; i++) {
        ejecutarPaso();

        int progresoActual = (i * 100 / totalPasos);
        if (progresoActual >= progresoAnterior + 10) {
            cout << "Progreso: " << progresoActual << "% (t="
                 << fixed << setprecision(2) << tiempoActual
                 << "s, partículas=" << contarParticulasActivas() << ")" << endl;
            progresoAnterior = progresoActual;
        }

        // Detener si solo queda una partícula
        if (contarParticulasActivas() <= 1) {
            cout << "\nSimulación detenida: solo queda una partícula." << endl;
            break;
        }

        if (verificarEstancamiento()) {
            cout << "\nAdvertencia: simulación estancada." << endl;
            break;
        }
    }
}

void Simulador::finalizar() {
    cerrarArchivos();
    mostrarEstadisticas();

    if (motorColisiones) {
        motorColisiones->mostrarEstadisticas();
    }

    cout << "===============================================" << endl;
    cout << "              SIMULACIÓN FINALIZADA             " << endl;
    cout << "===============================================" << endl;
}

void Simulador::actualizarPosiciones() {
    for (Particula* p : particulas) {
        if (p->estaActiva()) {
            p->mover(dt);
        }
    }
}

void Simulador::detectarYResolverColisiones() {
    // ORDEN IMPORTANTE:
    // 1. Colisiones con paredes (elásticas)
    detectarColisionesParedes();

    // 2. Colisiones con obstáculos (inelásticas)
    detectarColisionesObstaculos();

    // 3. Colisiones entre partículas (fusión)
    detectarColisionesEntreParticulas();
}

void Simulador::detectarColisionesParedes() {
    // COLISIONES ELÁSTICAS: inversión de velocidad perpendicular
    for (Particula* p : particulas) {
        if (!p->estaActiva()) continue;

        Vector pos = p->getPosicion();
        double r = p->getRadio();

        if (pos.getX() - r <= 0 || pos.getX() + r >= ancho ||
            pos.getY() - r <= 0 || pos.getY() + r >= alto) {

            p->colisionarPared(ancho, alto);  // Ya implementa colisión elástica
            totalColisionesParedes++;
            registrarColision("PARED", p->getId());
        }
    }
}

void Simulador::detectarColisionesObstaculos() {
    // COLISIONES INELÁSTICAS con coeficiente de restitución
    for (Particula* p : particulas) {
        if (!p->estaActiva()) continue;

        for (Obstaculo& obs : obstaculos) {
            if (obs.colisionaCon(*p)) {
                Vector vel = p->getVelocidad();
                if (vel.magnitud() > 0.1) {
                    // Usa ColisionManager que aplica coeficiente de restitución
                    ColisionManager::colisionInelastica(*p, obs);
                    totalColisionesObstaculos++;
                    registrarColision("OBSTACULO", p->getId());
                }
                break;
            }
        }
    }
}

void Simulador::detectarColisionesEntreParticulas() {
    // COLISIONES COMPLETAMENTE INELÁSTICAS: fusión de partículas
    int fusionesEnEstePaso = 0;
    const int MAX_FUSIONES_POR_PASO = 1;  // Procesar una fusión a la vez

    for (size_t i = 0; i < particulas.size() && fusionesEnEstePaso < MAX_FUSIONES_POR_PASO; i++) {
        if (!particulas[i]->estaActiva()) continue;

        for (size_t j = i + 1; j < particulas.size(); j++) {
            if (!particulas[j]->estaActiva()) continue;

            if (particulas[i]->colisionaCon(*particulas[j])) {
                fusionarParticulas(particulas[i], particulas[j]);
                fusionesEnEstePaso++;
                break;
            }
        }
    }
}

void Simulador::fusionarParticulas(Particula* p1, Particula* p2) {
    auto* motorFusion = dynamic_cast<ColisionCompletamenteInelastica*>(motorColisiones);

    if (motorFusion) {
        // Separar partículas antes de fusionar
        ColisionManager::separarParticulas(*p1, *p2);

        // Crear nueva partícula fusionada
        Particula* nueva = motorFusion->fusionarParticulas(*p1, *p2);
        siguienteIdParticula = nueva->getId() + 1;
        motorFusion->setSiguienteId(siguienteIdParticula);

        // Desactivar partículas originales
        p1->setActiva(false);
        p2->setActiva(false);

        // Agregar nueva partícula
        particulas.push_back(nueva);

        totalColisionesParticulas++;
        registrarColision("FUSION", p1->getId(), p2->getId());

        cout << "  Fusión: P" << p1->getId() << " + P" << p2->getId()
             << " → P" << nueva->getId()
             << " (m=" << fixed << setprecision(2) << nueva->getMasa()
             << ", r=" << nueva->getRadio()
             << ", t=" << setprecision(3) << tiempoActual << "s)" << endl;
    }
}

void Simulador::guardarEstadoActual() {
    for (Particula* p : particulas) {
        if (p->estaActiva()) {
            int id = p->getId();
            Vector pos = p->getPosicion();

            if (archivosTrayectorias.find(id) == archivosTrayectorias.end()) {
                ofstream* nuevoArchivo = new ofstream();
                nuevoArchivo->open("trayectoria_" + to_string(id) + ".txt");
                archivosTrayectorias[id] = nuevoArchivo;
            }

            *archivosTrayectorias[id] << fixed << setprecision(3)
                                      << pos.getX() << " " << pos.getY() << endl;
        }
    }
}

void Simulador::registrarColision(const string& tipo, int id1, int id2) {
    archivoColisiones << fixed << setprecision(3)
    << tiempoActual << " " << tipo << " " << id1;
    if (id2 != -1)
        archivoColisiones << " " << id2;
    archivoColisiones << endl;
}

void Simulador::abrirArchivos() {
    archivoColisiones.open("colisiones.txt");
    if (!archivoColisiones.is_open()) {
        cerr << "Error al abrir archivo de colisiones" << endl;
    }
    archivoColisiones << "# tiempo tipo id1 [id2]" << endl;
}

void Simulador::cerrarArchivos() {
    if (archivoColisiones.is_open()) archivoColisiones.close();

    for (auto& par : archivosTrayectorias) {
        if (par.second && par.second->is_open()) {
            par.second->close();
            delete par.second;
        }
    }
    archivosTrayectorias.clear();
}

void Simulador::limpiarParticulasInactivas() {
    // No eliminar de memoria, solo mantener marcadas como inactivas
}

int Simulador::contarParticulasActivas() const {
    int count = 0;
    for (const Particula* p : particulas) {
        if (p->estaActiva()) count++;
    }
    return count;
}

void Simulador::mostrarEstadisticas() const {
    cout << endl;
    cout << "===============================================" << endl;
    cout << "             ESTADÍSTICAS FINALES              " << endl;
    cout << "===============================================" << endl;
    cout << "Tiempo simulado: " << fixed << setprecision(2) << tiempoActual << " s" << endl;
    cout << "Pasos ejecutados: " << pasoActual << endl;
    cout << "Partículas activas: " << contarParticulasActivas()
         << " / " << particulas.size() << endl;
    cout << endl;
    cout << "COLISIONES DETECTADAS:" << endl;
    cout << "  • Con paredes (elásticas): " << totalColisionesParedes << endl;
    cout << "  • Con obstáculos (inelásticas): " << totalColisionesObstaculos << endl;
    cout << "  • Fusiones de partículas: " << totalColisionesParticulas << endl;
    cout << "  • Total: " << (totalColisionesParedes + totalColisionesObstaculos + totalColisionesParticulas) << endl;
}

bool Simulador::verificarEstancamiento() const {
    return contadorPasosEstancado > 1000;
}
