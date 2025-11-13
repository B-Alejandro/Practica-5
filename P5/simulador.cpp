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

    setTipoColision(tipo, coefRestitucion);
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

void Simulador::setTipoColision(TipoColision tipo, double coefRestitucion) {
    if (motorColisiones) {
        delete motorColisiones;
        motorColisiones = nullptr;
    }

    tipoColisionActual = tipo;

    switch (tipo) {
    case TipoColision::ELASTICA:
        motorColisiones = new ColisionElastica();
        break;
    case TipoColision::INELASTICA:
        motorColisiones = new ColisionInelastica(coefRestitucion);
        break;
    case TipoColision::COMPLETAMENTE_INELASTICA:
        motorColisiones = new ColisionCompletamenteInelastica(siguienteIdParticula);
        break;
    }
}

TipoColision Simulador::getTipoColision() const {
    return tipoColisionActual;
}

void Simulador::agregarParticula(double x, double y, double vx, double vy,
                                 double masa, double radio) {
    Particula* nueva = new Particula(siguienteIdParticula, x, y, vx, vy, masa, radio);
    particulas.push_back(nueva);
    siguienteIdParticula++;

    if (tipoColisionActual == TipoColision::COMPLETAMENTE_INELASTICA) {
        auto* motorFusion = dynamic_cast<ColisionCompletamenteInelastica*>(motorColisiones);
        if (motorFusion) {
            motorFusion->setSiguienteId(siguienteIdParticula);
        }
    }
}

void Simulador::agregarObstaculo(double x, double y, double lado, double coefRestitucion) {
    obstaculos.emplace_back(x, y, lado, coefRestitucion);
}

void Simulador::configurarObstaculos(int cantidad) {
    double coef = 0.7;
    double lado = 30.0;

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
    cout << "             INICIO DE SIMULACION              " << endl;
    cout << "===============================================" << endl;
    cout << "Particulas: " << particulas.size() << endl;
    cout << "Obstaculos: " << obstaculos.size() << endl;
    cout << "Dimensiones: " << ancho << " x " << alto << endl;
    cout << "dt: " << dt << endl;

    cout << "Tipo de colision: ";
    switch (tipoColisionActual) {
    case TipoColision::ELASTICA:
        cout << "ELASTICA (e=1.0)" << endl;
        break;
    case TipoColision::INELASTICA:
        cout << "INELASTICA (e="
             << dynamic_cast<ColisionInelastica*>(motorColisiones)->getCoeficienteRestitucion()
             << ")" << endl;
        break;
    case TipoColision::COMPLETAMENTE_INELASTICA:
        cout << "COMPLETAMENTE INELASTICA (fusion)" << endl;
        break;
    }
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
                 << "s, particulas=" << contarParticulasActivas() << ")" << endl;
            progresoAnterior = progresoActual;
        }

        if (tipoColisionActual == TipoColision::COMPLETAMENTE_INELASTICA) {
            if (contarParticulasActivas() <= 1) {
                cout << "\nSimulacion detenida: solo queda una particula." << endl;
                break;
            }
        }

        if (verificarEstancamiento()) {
            cout << "\nAdvertencia: simulacion estancada." << endl;
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
    cout << "              SIMULACION FINALIZADA             " << endl;
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
    detectarColisionesParedes();
    detectarColisionesObstaculos();
    detectarColisionesEntreParticulas();
}

void Simulador::detectarColisionesParedes() {
    for (Particula* p : particulas) {
        if (!p->estaActiva()) continue;

        Vector pos = p->getPosicion();
        double r = p->getRadio();

        if (pos.getX() - r <= 0 || pos.getX() + r >= ancho ||
            pos.getY() - r <= 0 || pos.getY() + r >= alto) {

            resolverColisionPared(p);
            totalColisionesParedes++;
            registrarColision("PARED", p->getId());
        }
    }
}

void Simulador::detectarColisionesObstaculos() {
    for (Particula* p : particulas) {
        if (!p->estaActiva()) continue;

        for (Obstaculo& obs : obstaculos) {
            if (obs.colisionaCon(*p)) {
                Vector vel = p->getVelocidad();
                if (vel.magnitud() > 0.1) {
                    resolverColisionObstaculo(p, obs);
                    totalColisionesObstaculos++;
                    registrarColision("OBSTACULO", p->getId());
                }
                break;
            }
        }
    }
}

void Simulador::detectarColisionesEntreParticulas() {
    if (tipoColisionActual == TipoColision::COMPLETAMENTE_INELASTICA) {
        int fusionesEnEstePaso = 0;
        const int MAX_FUSIONES_POR_PASO = 1;

        for (size_t i = 0; i < particulas.size() && fusionesEnEstePaso < MAX_FUSIONES_POR_PASO; i++) {
            if (!particulas[i]->estaActiva()) continue;

            for (size_t j = i + 1; j < particulas.size(); j++) {
                if (!particulas[j]->estaActiva()) continue;

                if (particulas[i]->colisionaCon(*particulas[j])) {
                    resolverColisionEntreParticulas(particulas[i], particulas[j]);
                    fusionesEnEstePaso++;
                    break;
                }
            }
        }
    } else {
        for (size_t i = 0; i < particulas.size(); i++) {
            if (!particulas[i]->estaActiva()) continue;

            for (size_t j = i + 1; j < particulas.size(); j++) {
                if (!particulas[j]->estaActiva()) continue;

                if (particulas[i]->colisionaCon(*particulas[j])) {
                    motorColisiones->resolverColision(*particulas[i], *particulas[j]);
                    totalColisionesParticulas++;
                    registrarColision("PARTICULA", particulas[i]->getId(), particulas[j]->getId());
                }
            }
        }
    }
}

void Simulador::resolverColisionPared(Particula* p) {
    p->colisionarPared(ancho, alto);
}

void Simulador::resolverColisionObstaculo(Particula* p, Obstaculo& obs) {
    ColisionManager::colisionInelastica(*p, obs);
}

void Simulador::resolverColisionEntreParticulas(Particula* p1, Particula* p2) {
    if (tipoColisionActual == TipoColision::COMPLETAMENTE_INELASTICA) {
        auto* motorFusion = dynamic_cast<ColisionCompletamenteInelastica*>(motorColisiones);

        if (motorFusion) {
            ColisionManager::separarParticulas(*p1, *p2);

            Particula* nueva = motorFusion->fusionarParticulas(*p1, *p2);
            siguienteIdParticula = nueva->getId() + 1;
            motorFusion->setSiguienteId(siguienteIdParticula);

            p1->setActiva(false);
            p2->setActiva(false);
            particulas.push_back(nueva);

            totalColisionesParticulas++;
            registrarColision("FUSION", p1->getId(), p2->getId());

            cout << "Fusion: P" << p1->getId() << " + P" << p2->getId()
                 << " -> P" << nueva->getId()
                 << " (t=" << fixed << setprecision(3) << tiempoActual << "s)" << endl;
        }
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
    // No eliminar de memoria, solo marcar inactivas
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
    cout << "             ESTADISTICAS DE SIMULACION         " << endl;
    cout << "===============================================" << endl;
    cout << "Tiempo simulado: " << fixed << setprecision(2) << tiempoActual << " s" << endl;
    cout << "Pasos ejecutados: " << pasoActual << endl;
    cout << "Particulas activas: " << contarParticulasActivas()
         << " / " << particulas.size() << endl;
    cout << "Colisiones con paredes: " << totalColisionesParedes << endl;
    cout << "Colisiones con obstaculos: " << totalColisionesObstaculos << endl;
    cout << "Colisiones entre particulas: " << totalColisionesParticulas << endl;
}

bool Simulador::verificarEstancamiento() const {
    return contadorPasosEstancado > 1000;
}
