#include "proyectil.h"
#include "obstaculo.h"
#include "jugador.h"
#include <QtMath>
#include <QGraphicsScene>
#include <QBrush>
#include <QDebug>

Proyectil::Proyectil(double x0, double y0, double velocidad, double angulo, int jugadorPropietario, double gravedad)
    : QObject()
    , QGraphicsEllipseItem(-10, -10, 20, 20)
    , rebotesRestantes(3)
{
    qDebug() << ">>> Proyectil CONSTRUCTOR iniciado";

    xInicial = x0;
    yInicial = y0;
    v0 = velocidad * 3;
    g = gravedad;
    tiempo = 0.0;
    impacto = false;
    destruyendo = false;
    eventoEmitido = false;
    jugadorOwner = jugadorPropietario;

    angRad = qDegreesToRadians(angulo);

    // Calcular velocidades iniciales
    velocidadX = v0 * qCos(angRad);
    velocidadY = -v0 * qSin(angRad);

    // ********************************************
    // *** CÓDIGO MEJORADO PARA LA APARIENCIA ***
    // ********************************************
    QRadialGradient gradiente(0, 0, 10, 3, 3); // Centro: (3, 3), Radio: 10 (coordenadas locales)
    gradiente.setColorAt(0, QColor(100, 100, 100)); // Brillo
    gradiente.setColorAt(0.5, QColor(50, 50, 50));
    gradiente.setColorAt(1, QColor(10, 10, 10));// Sombra
    setBrush(QBrush(gradiente));
    setPen(QPen(Qt::darkGray, 1)); // Borde sutil
    // ********************************************

    setPos(xInicial, yInicial);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Proyectil::actualizar);
    timer->start(16);

    qDebug() << ">>> Proyectil creado. Velocidad real:" << v0 << "Ángulo:" << angulo;
}

Proyectil::~Proyectil()
{
    qDebug() << ">>> Proyectil DESTRUCTOR llamado" << static_cast<void*>(this);
}

// NUEVA FUNCIÓN: Calcula la magnitud del vector velocidad actual
double Proyectil::getVelocidadActual() const
{
    // Magnitud del vector: ||v|| = sqrt(vx^2 + vy^2)
    return qSqrt(velocidadX * velocidadX + velocidadY * velocidadY);
}

// MODIFICACIÓN: El daño depende de la velocidad actual
int Proyectil::calcularDano() const
{
    double velocidad = getVelocidadActual();
    int danoBase = 1;

    // Escalar el daño basado en la velocidad (ejemplo: cada 10 unidades de velocidad base, +1 daño)
    // El v0 inicial está multiplicado por 3, por lo que una velocidad de 20 (base) * 3 = 60
    // Usamos una función que crece con la velocidad.
    // Dano = 1 + floor(Velocidad / Factor_Escala)
    const double FACTOR_ESCALA = 10.0;
    int danoCalculado = danoBase + qFloor(velocidad / FACTOR_ESCALA);

    // Limitar el daño máximo a un valor razonable (ej. 5 o 6)
    const int MAX_DANO = 6;
    if (danoCalculado > MAX_DANO)
        danoCalculado = MAX_DANO;

    qDebug() << ">>> Velocidad actual para daño:" << velocidad << "Daño calculado:" << danoCalculado;
    return danoCalculado;
}

void Proyectil::procesarRebote()
{
    // ... (Resto de la función sin cambios)

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
    // ... (Resto de la función sin cambios)
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
    // ... (Resto de la función sin cambios)
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

    double x = pos().x() + velocidadX * 0.016;  // Multiplicar por dt
    double y = pos().y() + velocidadY * 0.016;  // Multiplicar por dt

    double anchoEscena = escena->width();
    double altoEscena = escena->height();
    double sueloY = altoEscena * 0.9;

    // REBOTE EN SUELO
    if (y >= sueloY - 10 && velocidadY > 0)  // Ajustado a -10 por el tamaño del proyectil
    {
        qDebug() << ">>> Rebote en suelo detectado en Y:" << y << "SueloY:" << sueloY;
        setPos(x, sueloY - 10);
        procesarRebote();
        return;
    }
    // ... (Resto de la lógica de rebote en paredes y techo)
    // REBOTE EN PAREDES
    if (x <= 10 && velocidadX < 0)  // Ajustado por el radio del proyectil
    {
        qDebug() << ">>> Rebote en pared izquierda";
        velocidadX = -velocidadX * 0.8;
        x = 10;
    }
    else if (x >= anchoEscena - 10 && velocidadX > 0)  // Ajustado por el radio del proyectil
    {
        qDebug() << ">>> Rebote en pared derecha";
        velocidadX = -velocidadX * 0.8;
        x = anchoEscena - 10;
    }

    // REBOTE EN TECHO
    if (y <= 10 && velocidadY < 0)  // Ajustado por el radio del proyectil
    {
        qDebug() << ">>> Rebote en techo";
        velocidadY = -velocidadY * 0.8;
        y = 10;
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

    // Detección de colisión - MEJORADA
    QList<QGraphicsItem*> colisiones = collidingItems();

    for (QGraphicsItem *item : colisiones)
    {
        if (!item)
            continue;

        // Verificar colisión con jugadores
        Jugador *jugador = dynamic_cast<Jugador*>(item);
        if (jugador && !impacto)
        {
            int jugadorGolpeadoNum = jugador->getNumeroJugador();

            qDebug() << ">>> Colisión detectada con jugador" << jugadorGolpeadoNum;

            // No golpear al jugador que disparó
            if (jugadorGolpeadoNum != jugadorOwner || rebotesRestantes < 3)
            {
                impacto = true;

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

            int d = calcularDano(); // El daño ahora depende de la velocidad
            o->recibirDano(d);
            qDebug() << ">>> Daño aplicado:" << d << "Vida restante:" << o->obtenerVida();

            if (o->obtenerVida() <= 0)
            {
                qDebug() << ">>> Obstáculo destruido por falta de vida";
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
