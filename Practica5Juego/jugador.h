#ifndef JUGADOR_H
#define JUGADOR_H

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QKeyEvent>

class Juego; // Declaración adelantada para evitar inclusión circular
class Proyectil; // Declaración adelantada

class Jugador : public QGraphicsPixmapItem
{
public:
    Jugador(Juego *juego, QGraphicsScene *escena, int numeroJugador);

    void cargarAnimacion();
    void actualizarFrame();
    void verificarColisionSuelo();

    void keyPressEvent(QKeyEvent *event) override;
    void lanzar();
    void perderVida();
    void resetear();

    double getAngulo() const { return angulo; }
    double getVelocidad() const { return velocidad; }
    int getVidas() const { return vidas; }
    int getNumeroJugador() const { return numJugador; }


private:
    Juego *juegoPtr;
    QGraphicsScene *escenaPtr;
    int numJugador;

    QPixmap baseCannonSprite; // Un solo QPixmap para el sprite base del cañón

    double angulo;
    double velocidad;
    int vidas;
};

#endif // JUGADOR_H
