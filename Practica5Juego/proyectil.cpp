#include "proyectil.h"
#include "obstaculo.h"
#include "jugador.h"
#include <QtMath>
#include <QGraphicsScene>
#include <QBrush>
#include <QDebug>

Proyectil::Proyectil(double x0, double y0, double velocidad, double angulo, int jugadorPropietario, double gravedad)
    : QObject()
    , QGraphicsEllipseItem(-5, -5, 10, 10)
    , rebotesRestantes(3)
{
    qDebug() << ">>> Proyectil CONSTRUCTOR iniciado";

    xInicial = x0;
    yInicial = y0;
    v0 = velocidad;
    g = gravedad;
    tiempo = 0.0;
    impacto = false;
    destruyendo = false;
    eventoEmitido = false;
    jugadorOwner = jugadorPropietario;

    angRad = qDegreesToRadians(angulo);

    // Calcular velocidades iniciales
    velocidadX = v0 * qCos(angRad);
    velocidadY = -v0 * qSin(angRad); // Negativo porque Y crece hacia abajo

    setBrush(QBrush(Qt::black));
    setPos(xInicial, yInicial);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Proyectil::actualizar);
    timer->start(16);

    qDebug() << ">>> Proyectil CONSTRUCTOR completado" << static_cast<void*>(this);
}

Proyectil::~Proyectil()
{
    qDebug() << ">>> Proyectil DESTRUCTOR llamado" << static_cast<void*>(this);
}

int Proyectil::calcularDano() const
{
    return 1;
}

void Proyectil::procesarRebote()
{
    qDebug() << ">>> Rebote! Rebotes restantes:" << rebotesRestantes;

    rebotesRestantes--;

    if (rebotesRestantes == 1)
    {
        qDebug() << ">>> Segundo rebote detectado - Contando como fallo";
        if (!impacto)
        {
            emit impactoDetectado(false);
        }
        detenerYDestruir();
        return;
    }

    velocidadY = -velocidadY * 0.7;
    velocidadX *= 0.85;

    if (rebotesRestantes <= 0)
    {
        qDebug() << ">>> Sin rebotes restantes, deteniendo";
        if (!impacto)
        {
            emit impactoDetectado(false);
        }
        detenerYDestruir();
    }
}

void Proyectil::detenerYDestruir()
{
    qDebug() << ">>> detenerYDestruir llamado, destruyendo:" << destruyendo;

    if (destruyendo)
    {
        qDebug() << ">>> Ya estaba destruyendo, saliendo";
        return;
    }

    destruyendo = true;

    if (timer)
    {
        qDebug() << ">>> Deteniendo timer";
        if (timer->isActive())
            timer->stop();
    }

    if (!eventoEmitido)
    {
        qDebug() << ">>> Emitiendo proyectilFinalizado";
        eventoEmitido = true;
        emit proyectilFinalizado();
    }

    qDebug() << ">>> Programando eliminación del proyectil";
    QTimer::singleShot(0, this, [this]() {
        qDebug() << ">>> Lambda de eliminación ejecutándose";
        QGraphicsScene* escenaActual = scene();
        if (escenaActual)
        {
            qDebug() << ">>> Removiendo de escena";
            escenaActual->removeItem(this);
        }
        qDebug() << ">>> Llamando deleteLater";
        deleteLater();
    });
}

void Proyectil::actualizar()
{
    if (destruyendo || eventoEmitido)
        return;

    QGraphicsScene* escena = scene();
    if (!escena)
    {
        qDebug() << ">>> actualizar: escena es nullptr";
        detenerYDestruir();
        return;
    }

    tiempo += 0.016;

    velocidadY += g * 0.016;

    double x = pos().x() + velocidadX;
    double y = pos().y() + velocidadY;

    double anchoEscena = escena->width();
    double altoEscena = escena->height();
    double sueloY = altoEscena * 0.9; // Mismo cálculo que en Juego

    // REBOTE EN SUELO - Corregido para usar la posición correcta del suelo
    if (y >= sueloY - 5 && velocidadY > 0)
    {
        qDebug() << ">>> Rebote en suelo detectado en Y:" << y << "SueloY:" << sueloY;
        setPos(x, sueloY - 5);
        procesarRebote();
        return;
    }

    // REBOTE EN PAREDES
    if (x <= 0 && velocidadX < 0)
    {
        qDebug() << ">>> Rebote en pared izquierda";
        velocidadX = -velocidadX * 0.8;
        x = 0;
    }
    else if (x >= anchoEscena && velocidadX > 0)
    {
        qDebug() << ">>> Rebote en pared derecha";
        velocidadX = -velocidadX * 0.8;
        x = anchoEscena;
    }

    // REBOTE EN TECHO
    if (y <= 0 && velocidadY < 0)
    {
        qDebug() << ">>> Rebote en techo";
        velocidadY = -velocidadY * 0.8;
        y = 0;
    }

    setPos(x, y);

    if (x > anchoEscena + 100 || y > altoEscena + 100 || x < -100 || y < -100)
    {
        qDebug() << ">>> Proyectil fuera de escena en pos:" << x << y;
        if (!impacto && !eventoEmitido)
        {
            emit impactoDetectado(false);
        }
        detenerYDestruir();
        return;
    }

    // Detección de colisión
    QList<QGraphicsItem*> colisiones = collidingItems();

    for (QGraphicsItem *item : colisiones)
    {
        if (!item)
            continue;

        // Verificar colisión con jugadores
        Jugador *jugador = dynamic_cast<Jugador*>(item);
        if (jugador && !impacto)
        {
            qDebug() << ">>> Colisión detectada con jugador";
            impacto = true;

            // Determinar qué jugador fue golpeado comparando punteros
            int jugadorGolpeadoNum = 0;

            // Necesitamos identificar al jugador mediante su posición o una marca
            // Por simplicidad, usamos la posición X
            if (jugador->x() < escena->width() / 2) {
                jugadorGolpeadoNum = 2; // Jugador 2 está a la izquierda
            } else {
                jugadorGolpeadoNum = 1; // Jugador 1 está a la derecha
            }

            // No golpear al jugador que disparó
            if (jugadorGolpeadoNum != jugadorOwner)
            {
                if (!eventoEmitido)
                {
                    eventoEmitido = true;
                    emit jugadorGolpeado(jugadorGolpeadoNum);
                    emit impactoDetectado(true);
                }

                detenerYDestruir();
                return;
            }
        }

        // Verificar colisión con obstáculos
        Obstaculo *o = dynamic_cast<Obstaculo*>(item);
        if (o && !impacto)
        {
            qDebug() << ">>> Colisión detectada con obstáculo";
            impacto = true;

            int d = calcularDano();
            o->recibirDano(d);
            qDebug() << ">>> Daño aplicado:" << d << "Vida restante:" << o->obtenerVida();

            if (o->obtenerVida() <= 0)
            {
                qDebug() << ">>> Obstáculo destruido por falta de vida";
                o->setBrush(QBrush(QColor(50, 50, 50)));
                QGraphicsScene* escenaObs = o->scene();
                if (escenaObs)
                {
                    escenaObs->removeItem(o);
                    delete o;
                }
            }

            if (!eventoEmitido)
            {
                eventoEmitido = true;
                emit impactoDetectado(true);
            }

            detenerYDestruir();
            return;
        }
    }
}
