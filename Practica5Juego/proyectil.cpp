#include "proyectil.h"
#include "obstaculo.h"
#include <QtMath>
#include <QGraphicsScene>
#include <QBrush>
#include <QDebug>

Proyectil::Proyectil(double x0, double y0, double velocidad, double angulo, double gravedad)
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

    angRad = qDegreesToRadians(angulo);

    // Calcular velocidades iniciales basadas en v0 y ángulo
    velocidadX = v0 * qCos(angRad);
    velocidadY = v0 * qSin(angRad);

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

    // El segundo rebote (rebotesRestantes == 1) se cuenta como fallo
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

    // Invertir velocidad Y (rebote vertical) con pérdida de energía
    velocidadY = -velocidadY * 0.7;

    // Reducir velocidad X por fricción
    velocidadX *= 0.85;

    // Si no hay más rebotes, detener
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

    // Emitir señal de finalización
    if (!eventoEmitido)
    {
        qDebug() << ">>> Emitiendo proyectilFinalizado";
        eventoEmitido = true;
        emit proyectilFinalizado();
    }

    // Programar eliminación
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

    // Aplicar gravedad a la velocidad Y
    velocidadY += g * 0.016;

    // Calcular nueva posición
    double x = pos().x() + velocidadX;
    double y = pos().y() + velocidadY;

    double anchoEscena = escena->width();
    double altoEscena = escena->height();

    // REBOTE EN SUELO (Y = 550)
    if (y >= 550 && velocidadY > 0)
    {
        qDebug() << ">>> Rebote en suelo detectado";
        setPos(x, 550);
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

    // Verificar si sale de la escena
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

    // Detección de colisión con obstáculos
    QList<QGraphicsItem*> colisiones = collidingItems();

    for (QGraphicsItem *item : colisiones)
    {
        if (!item)
            continue;

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
