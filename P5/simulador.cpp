/**
 * @file simulador.cpp
 * @brief Implementación de la clase Simulador, encargada de ejecutar la simulación
 *        física de partículas dentro de una caja con obstáculos, paredes y
 *        colisiones entre partículas.
 *
 * Este módulo administra la evolución temporal del sistema mediante un bucle
 * discreto en pasos de tiempo (dt). Se encarga de:
 *  - Actualizar posiciones de partículas.
 *  - Detectar y resolver colisiones con paredes, obstáculos y otras partículas.
 *  - Registrar eventos en archivos de salida (trayectorias y colisiones).
 *  - Manejar fusiones inelásticas entre partículas.
 *  - Controlar el estado general de la simulación y evitar bloqueos o estancamientos.
 */

#include "simulador.h"
#include <iostream>
#include <iomanip>

using namespace std;

/**
 * @brief Constructor del simulador.
 * @param ancho Ancho de la caja de simulación.
 * @param alto Alto de la caja de simulación.
 * @param dt Intervalo de tiempo discreto entre pasos de simulación.
 *
 * Inicializa todos los contadores, el tiempo, los registros y las estructuras
 * de almacenamiento de partículas y obstáculos.
 */
Simulador::Simulador(double ancho, double alto, double dt)
    : ancho(ancho), alto(alto), dt(dt), tiempoActual(0.0),
    tiempoTotal(0.0), pasoActual(0),
    totalColisionesParticulas(0), totalColisionesObstaculos(0),
    totalColisionesParedes(0), contadorPasosEstancado(0),
    ultimoNumParticulas(0), siguienteIdParticula(0) {}

/**
 * @brief Destructor del simulador.
 *
 * Libera la memoria dinámica asociada a las partículas y cierra todos los archivos
 * de salida abiertos durante la simulación.
 */
Simulador::~Simulador() {
    for (Particula* p : particulas) {
        delete p;
    }
    particulas.clear();
    cerrarArchivos();
}

/**
 * @brief Agrega una nueva partícula al sistema.
 * @param x Posición inicial en X.
 * @param y Posición inicial en Y.
 * @param vx Velocidad inicial en X.
 * @param vy Velocidad inicial en Y.
 * @param masa Masa de la partícula.
 * @param radio Radio de la partícula.
 */
void Simulador::agregarParticula(double x, double y, double vx, double vy,
                                 double masa, double radio) {
    Particula* nueva = new Particula(siguienteIdParticula, x, y, vx, vy, masa, radio);
    particulas.push_back(nueva);
    siguienteIdParticula++;
}

/**
 * @brief Agrega un obstáculo cuadrado al sistema.
 * @param x Coordenada X de la esquina superior izquierda.
 * @param y Coordenada Y de la esquina superior izquierda.
 * @param lado Tamaño del lado del cuadrado.
 * @param coefRestitucion Coeficiente de restitución del obstáculo (inelasticidad).
 */
void Simulador::agregarObstaculo(double x, double y, double lado, double coefRestitucion) {
    Obstaculo obs(x, y, lado, coefRestitucion);
    obstaculos.push_back(obs);
}

/**
 * @brief Configura cuatro obstáculos por defecto en los cuadrantes de la caja.
 *
 * Cada obstáculo se ubica en una posición simétrica dentro del área de simulación
 * con un coeficiente de restitución fijo.
 */
void Simulador::configurarObstaculosDefecto() {
    double coef = 0.7;
    double lado = 30.0;

    agregarObstaculo(ancho * 0.25, alto * 0.25, lado, coef);
    agregarObstaculo(ancho * 0.75, alto * 0.25, lado, coef);
    agregarObstaculo(ancho * 0.25, alto * 0.75, lado, coef);
    agregarObstaculo(ancho * 0.75, alto * 0.75, lado, coef);
}

/**
 * @brief Inicializa la simulación.
 *
 * Reinicia contadores, abre archivos de salida y muestra información
 * general del sistema.
 */
void Simulador::iniciar() {
    tiempoActual = 0.0;
    pasoActual = 0;
    abrirArchivos();

    cout << "=== SIMULACIÓN INICIADA ===" << endl;
    cout << "Partículas: " << particulas.size() << endl;
    cout << "Obstáculos: " << obstaculos.size() << endl;
    cout << "Dimensiones: " << ancho << " x " << alto << endl;
    cout << "dt: " << dt << endl;
}

/**
 * @brief Ejecuta un paso de simulación.
 *
 * Actualiza posiciones, detecta y resuelve colisiones, guarda el estado,
 * y monitorea el posible estancamiento del sistema.
 */
void Simulador::ejecutarPaso() {
    int particuasActivasAntes = contarParticulasActivas();

    actualizarPosiciones();
    detectarYResolverColisiones();
    guardarEstadoActual();
    limpiarParticulasInactivas();

    // Detección de estancamiento
    int particuasActivasAhora = contarParticulasActivas();
    if (particuasActivasAhora == ultimoNumParticulas) {
        contadorPasosEstancado++;
    } else {
        contadorPasosEstancado = 0;
        ultimoNumParticulas = particuasActivasAhora;
    }

    tiempoActual += dt;
    pasoActual++;
}

/**
 * @brief Ejecuta toda la simulación hasta un tiempo final dado.
 * @param tiempoFinal Tiempo total a simular (en segundos simulados).
 */
void Simulador::ejecutar(double tiempoFinal) {
    tiempoTotal = tiempoFinal;
    int totalPasos = static_cast<int>(tiempoFinal / dt);
    int progresoAnterior = -1;

    for (int i = 0; i < totalPasos; i++) {
        ejecutarPaso();

        // Mostrar progreso cada 10 %
        int progresoActual = (i * 100 / totalPasos);
        if (progresoActual >= progresoAnterior + 10) {
            cout << "Progreso: " << progresoActual << "% (t="
                 << tiempoActual << "s, partículas activas="
                 << contarParticulasActivas() << ")" << endl;
            progresoAnterior = progresoActual;
        }

        // Condición de parada: solo una partícula restante
        if (contarParticulasActivas() <= 1) {
            cout << "\nSimulación detenida: solo queda una partícula activa." << endl;
            break;
        }

        // Estancamiento prolongado
        if (verificarEstancamiento()) {
            cout << "\nADVERTENCIA: Simulación estancada. Deteniendo..." << endl;
            cout << "Posible causa: partículas atascadas o dt demasiado pequeño." << endl;
            break;
        }
    }
}

/**
 * @brief Finaliza la simulación, cierra archivos y muestra estadísticas.
 */
void Simulador::finalizar() {
    cerrarArchivos();
    mostrarEstadisticas();
    cout << "=== SIMULACIÓN FINALIZADA ===" << endl;
}

/**
 * @brief Actualiza las posiciones de todas las partículas activas según su velocidad.
 */
void Simulador::actualizarPosiciones() {
    for (Particula* p : particulas) {
        if (p->estaActiva()) {
            p->mover(dt);
        }
    }
}

/**
 * @brief Ejecuta todas las detecciones de colisiones en el sistema.
 *
 * Llama a los tres tipos de detección: paredes, obstáculos y partículas.
 */
void Simulador::detectarYResolverColisiones() {
    detectarColisionesParedes();
    detectarColisionesObstaculos();
    detectarColisionesEntreParticulas();
}

/**
 * @brief Detecta y resuelve colisiones de partículas con las paredes de la caja.
 *
 * Las colisiones con las paredes son perfectamente elásticas.
 */
void Simulador::detectarColisionesParedes() {
    for (Particula* p : particulas) {
        if (p->estaActiva()) {
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
}

/**
 * @brief Detecta y resuelve colisiones inelásticas entre partículas y obstáculos.
 */
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

/**
 * @brief Detecta y resuelve colisiones entre pares de partículas.
 *
 * Las colisiones entre partículas son completamente inelásticas (fusión).
 */
void Simulador::detectarColisionesEntreParticulas() {
    int fusionesEnEstePaso = 0;
    const int MAX_FUSIONES_POR_PASO = 1;

    for (size_t i = 0; i < particulas.size() && fusionesEnEstePaso < MAX_FUSIONES_POR_PASO; i++) {
        if (!particulas[i]->estaActiva()) continue;

        for (size_t j = i + 1; j < particulas.size(); j++) {
            if (!particulas[j]->estaActiva()) continue;

            if (particulas[i]->colisionaCon(*particulas[j])) {
                resolverColisionEntreParticulas(particulas[i], particulas[j]);
                totalColisionesParticulas++;
                registrarColision("PARTICULA", particulas[i]->getId(), particulas[j]->getId());
                fusionesEnEstePaso++;
                break;
            }
        }
    }
}

/**
 * @brief Resuelve una colisión elástica entre una partícula y una pared.
 */
void Simulador::resolverColisionPared(Particula* p) {
    p->colisionarPared(ancho, alto);
}

/**
 * @brief Resuelve una colisión inelástica entre una partícula y un obstáculo.
 */
void Simulador::resolverColisionObstaculo(Particula* p, Obstaculo& obs) {
    ColisionManager::colisionInelastica(*p, obs);
}

/**
 * @brief Resuelve una colisión completamente inelástica entre dos partículas.
 *
 * Las partículas se fusionan en una nueva partícula que conserva el momento lineal.
 */
void Simulador::resolverColisionEntreParticulas(Particula* p1, Particula* p2) {
    ColisionManager::separarParticulas(*p1, *p2);

    Particula* nueva = ColisionManager::fusionarParticulas(*p1, *p2, siguienteIdParticula);
    siguienteIdParticula++;

    p1->setActiva(false);
    p2->setActiva(false);
    particulas.push_back(nueva);

    cout << "Fusión: P" << p1->getId() << " + P" << p2->getId()
         << " -> P" << nueva->getId() << " (t=" << tiempoActual << "s)" << endl;
}

/**
 * @brief Guarda la posición actual (x, y) de cada partícula activa.
 *
 * Crea un archivo individual por partícula con nombre `trayectoria_<id>.txt`.
 */
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

/**
 * @brief Registra un evento de colisión en el archivo `colisiones.txt`.
 * @param tipo Tipo de colisión ("PARED", "OBSTACULO", "PARTICULA").
 * @param id1 ID de la primera partícula involucrada.
 * @param id2 ID de la segunda partícula (opcional, -1 si no aplica).
 */
void Simulador::registrarColision(const string& tipo, int id1, int id2) {
    archivoColisiones << fixed << setprecision(3)
    << tiempoActual << " " << tipo << " " << id1;
    if (id2 != -1)
        archivoColisiones << " " << id2;
    archivoColisiones << endl;
}

/**
 * @brief Abre los archivos de registro necesarios para la simulación.
 */
void Simulador::abrirArchivos() {
    archivoColisiones.open("colisiones.txt");
    if (!archivoColisiones.is_open()) {
        cerr << "Error al abrir archivo de colisiones" << endl;
    }
    archivoColisiones << "# tiempo tipo id1 [id2]" << endl;
}

/**
 * @brief Cierra todos los archivos abiertos de salida y libera memoria asociada.
 */
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

/**
 * @brief Cuenta cuántas partículas están activas actualmente en el sistema.
 * @return Número de partículas activas.
 */
int Simulador::contarParticulasActivas() const {
    int count = 0;
    for (const Particula* p : particulas) {
        if (p->estaActiva()) count++;
    }
    return count;
}

/**
 * @brief Muestra las estadísticas generales de la simulación.
 */
void Simulador::mostrarEstadisticas() const {
    cout << "\n=== ESTADÍSTICAS ===" << endl;
    cout << "Tiempo total simulado: " << tiempoActual << " s" << endl;
    cout << "Pasos ejecutados: " << pasoActual << endl;
    cout << "Partículas activas: " << contarParticulasActivas()
         << " / " << particulas.size() << endl;
    cout << "Colisiones con paredes: " << totalColisionesParedes << endl;
    cout << "Colisiones con obstáculos: " << totalColisionesObstaculos << endl;
    cout << "Colisiones entre partículas: " << totalColisionesParticulas << endl;
}

/**
 * @brief Verifica si el sistema lleva demasiado tiempo sin cambios (estancado).
 * @return true si el sistema está estancado, false en caso contrario.
 */
bool Simulador::verificarEstancamiento() const {
    return contadorPasosEstancado > 1000;
}
