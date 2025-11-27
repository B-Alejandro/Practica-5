#ifndef JUGADOR_H
#define JUGADOR_H

#include <QGraphicsPixmapItem>
#include <QKeyEvent>
#include <QPixmap>

class Juego;
class QGraphicsScene;

class Jugador : public QGraphicsPixmapItem
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

    QPixmap spriteSheet;
    int frameAncho;
    int frameAlto;
    int framesPoraAngulo;

    void lanzar();
    void cargarAnimacion();
    void actualizarFrame();
    int calcularFrameDelAngulo();
};

#endif // JUGADOR_H
