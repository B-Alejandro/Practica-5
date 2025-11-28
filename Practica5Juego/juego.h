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

public slots:
    void onImpactoProyectil(bool acerto);
    void onProyectilFinalizado();
    void onJugadorGolpeado(int jugadorNumero);

private slots:
    void onReiniciar();

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

    // Panel de información
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

    // Sistema de puntos y turnos
    int jugadorActual;
    int puntajeJ1;
    int puntajeJ2;
    bool turnoEnProceso;

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
};

#endif // JUEGO_H
