#ifndef JUEGO_H
#define JUEGO_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

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
    qreal getSueloY() const; // Nueva función para obtener la posición Y del suelo

public slots:
    void onImpactoProyectil(bool acerto);
    void onProyectilFinalizado();

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
    QLabel *labelFallosJ1;
    QLabel *labelFallosJ2;
    QPushButton *btnReiniciar;

    // Sistema de puntos y turnos
    int jugadorActual; // 1 o 2
    int puntajeJ1;
    int puntajeJ2;
    int fallosJ1;
    int fallosJ2;
    bool turnoEnProceso;

    // Variables para el suelo
    qreal sueloY;
    qreal alturaSuelo;

    void crearCasa();
    void registrarImpacto(bool acerto);
    void cambiarTurno();
    void verificarFinJuego();
    void mostrarGanador();
    void reiniciarJuego();
};

#endif // JUEGO_H
