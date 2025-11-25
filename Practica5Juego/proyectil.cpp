#include "proyectil.h"
#include "obstaculo.h"
#include <QtMath>
#include <QGraphicsScene>
#include <QBrush>
#include <QDebug>

Proyectil::Proyectil(double x0, double y0, double velocidad, double angulo, double gravedad)
    : QObject()
    , QGraphicsEllipseItem(-5, -5, 10, 10)
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
    double factor = qAbs(qSin(angRad));
    int dano = static_cast<int>(v0 * factor * 0.5);
    return qMax(dano, 5);
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

    double x = xInicial + v0 * qCos(angRad) * tiempo;
    double y = yInicial - (v0 * qSin(angRad) * tiempo - 0.5 * g * tiempo * tiempo);

    setPos(x, y);

    double anchoEscena = escena->width();
    double altoEscena = escena->height();

    // Verificar si sale de la escena
    if (x > anchoEscena + 100 || y > altoEscena + 100 || x < -100 || y < -100)
    {
        qDebug() << ">>> Proyectil fuera de escena en pos:" << x << y;
        if (!impacto && !eventoEmitido)
        {
            qDebug() << ">>> Emitiendo impactoDetectado(false)";
            eventoEmitido = true;
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
                o->setBrush(QBrush(QColor(50, 50, 50)));
            }

            if (!eventoEmitido)
            {
                qDebug() << ">>> Emitiendo impactoDetectado(true)";
                eventoEmitido = true;
                emit impactoDetectado(true);
            }

            detenerYDestruir();
            return;
        }
    }
}
