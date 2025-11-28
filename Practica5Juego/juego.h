#ifndef JUEGO_H
#define JUEGO_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QGraphicsPixmapItem>
#include "finjuegooverlay.h" // Se asume que este archivo .h existe

class Obstaculo;
class Jugador;

class Juego : public QMainWindow
{
    Q_OBJECT

public:
    explicit Juego(QWidget *parent = nullptr);

    QGraphicsScene* obtenerEscena() const;
    void actualizarPanel(double angulo, double velocidad);
    int getJugadorActual() const { return jugadorActual; }
    qreal getSueloY() const;
    Jugador* getJugador1() const { return jugador1; }
    Jugador* getJugador2() const { return jugador2; }

    // Función para manejar el lanzamiento y su restricción
    void lanzarProyectil(double angulo, double velocidad, qreal x, qreal y, int numJugador);

public slots:
    void onImpactoProyectil(bool acerto);
    void onProyectilFinalizado();
    void onJugadorGolpeado(int jugadorNumero);

private slots:
    void onReiniciar();
    void onReiniciarDesdeOverlay(); // Slot para la señal del overlay

private:
    QWidget *widgetCentral;
    QGraphicsView *vista;
    QGraphicsScene *escena;

    Jugador *jugador1;
    Jugador *jugador2;
    Jugador *jugadorActivo;

    // Obstáculos que forman la casa
    QList<Obstaculo*> obstaculosCasa;
    Obstaculo *obstaculoNPC;

    // Panel de información (labels y botón)
    QLabel *labelAngulo;
    QLabel *labelVelocidad;
    QLabel *labelJugador;
    QLabel *labelPuntajeJ1;
    QLabel *labelPuntajeJ2;
    QLabel *labelVidasJ1;
    QLabel *labelVidasJ2;
    QPushButton *btnReiniciar;

    // Corazones gráficos
    QList<QGraphicsPixmapItem*> corazonesJ1;
    QList<QGraphicsPixmapItem*> corazonesJ2;

    // Sistema de puntos y turnos (orden corregido para el warning -Wreorder)
    int jugadorActual;
    int puntajeJ1;
    int puntajeJ2;
    bool turnoEnProceso;

    // Flags de estado para control de juego (orden corregido para el warning -Wreorder)
    bool hitOccurredThisTurn;
    bool proyectilActivo;

    // Objetos de Interfaz de Fin de Juego (orden corregido para el warning -Wreorder)
    FinDeJuegoOverlay *finJuegoOverlay;
    QGraphicsPixmapItem *fondoGameOver;

    // Variables para el suelo
    qreal sueloY;
    qreal alturaSuelo;

    void crearCasa();
    void crearCorazones();
    void actualizarCorazones();
    void registrarImpacto(bool acerto);
    void cambiarTurno();
    void verificarFinJuego();
    void mostrarGanador();
    void reiniciarJuego();
    void cambiarFondoEscena(bool aGameOver); // Gestiona el fondo y visibilidad de elementos
};

#endif // JUEGO_H
