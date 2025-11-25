#ifndef JUGADOR_H
#define JUGADOR_H

#include <QGraphicsRectItem>
#include <QKeyEvent>

class Juego;
class QGraphicsScene;

class Jugador : public QGraphicsRectItem
{
public:
    Jugador(Juego *juego, QGraphicsScene *escena, int numeroJugador);

    double getAngulo() const { return angulo; }
    double getVelocidad() const { return velocidad; }
    void resetear();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    double angulo;
    double velocidad;
    int numJugador;

    Juego *juegoPtr;
    QGraphicsScene *escenaPtr;

    void lanzar();
};

#endif // JUGADOR_H
