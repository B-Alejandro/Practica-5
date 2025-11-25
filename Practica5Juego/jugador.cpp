#include "jugador.h"
#include "juego.h"
#include "proyectil.h"
#include <QtMath>
#include <QBrush>
#include <QGraphicsScene>
#include <QDebug>

Jugador::Jugador(Juego *juego, QGraphicsScene *escena, int numeroJugador)
    : QGraphicsRectItem(0, 0, 40, 30)
{
    qDebug() << ">>> Creando Jugador" << numeroJugador;

    juegoPtr = juego;
    escenaPtr = escena;
    numJugador = numeroJugador;

    angulo = 45.0;
    velocidad = 50.0;

    if (numeroJugador == 1)
        setBrush(QBrush(QColor(0, 100, 200)));
    else
        setBrush(QBrush(QColor(200, 0, 0)));

    setFlag(QGraphicsItem::ItemIsFocusable);

    qDebug() << ">>> Jugador" << numeroJugador << "constructor completado";
}

void Jugador::keyPressEvent(QKeyEvent *event)
{
    if (!event)
        return;

    qDebug() << ">>> Jugador keyPress:" << event->key();

    if (event->key() == Qt::Key_Up)
    {
        angulo += 1;
        if (angulo > 90)
            angulo = 90;
        if (juegoPtr)
            juegoPtr->actualizarPanel(angulo, velocidad);
    }
    else if (event->key() == Qt::Key_Down)
    {
        angulo -= 1;
        if (angulo < 0)
            angulo = 0;
        if (juegoPtr)
            juegoPtr->actualizarPanel(angulo, velocidad);
    }
    else if (event->key() == Qt::Key_Plus || event->key() == Qt::Key_Equal)
    {
        velocidad += 2;
        if (velocidad > 300)
            velocidad = 300;
        if (juegoPtr)
            juegoPtr->actualizarPanel(angulo, velocidad);
    }
    else if (event->key() == Qt::Key_Minus)
    {
        velocidad -= 2;
        if (velocidad < 10)
            velocidad = 10;
        if (juegoPtr)
            juegoPtr->actualizarPanel(angulo, velocidad);
    }
    else if (event->key() == Qt::Key_Space)
    {
        qDebug() << ">>> Lanzar proyectil!";
        lanzar();
    }
}

void Jugador::lanzar()
{
    qDebug() << ">>> lanzar() llamado";

    if (!escenaPtr)
    {
        qDebug() << ">>> ERROR: escenaPtr es nullptr!";
        return;
    }

    if (!juegoPtr)
    {
        qDebug() << ">>> ERROR: juegoPtr es nullptr!";
        return;
    }

    qDebug() << ">>> Creando proyectil en pos:" << (x() + 40) << (y() + 15);
    Proyectil *p = new Proyectil(x() + 40, y() + 15, velocidad, angulo);

    qDebug() << ">>> Conectando señales del proyectil...";
    QObject::connect(p, &Proyectil::impactoDetectado,
                     juegoPtr, &Juego::onImpactoProyectil);
    QObject::connect(p, &Proyectil::proyectilFinalizado,
                     juegoPtr, &Juego::onProyectilFinalizado);

    qDebug() << ">>> Agregando proyectil a la escena...";
    escenaPtr->addItem(p);

    qDebug() << ">>> Proyectil lanzado exitosamente";
}

void Jugador::resetear()
{
    qDebug() << ">>> Reseteando jugador" << numJugador;
    angulo = 45.0;
    velocidad = 50.0;
    setPos(50, 520);
}
