#include "jugador.h"
#include "juego.h"
#include "proyectil.h"
#include <QtMath>
#include <QBrush>
#include <QGraphicsScene>
#include <QDebug>
#include <QPixmap>
#include <QTransform>

Jugador::Jugador(Juego *juego, QGraphicsScene *escena, int numeroJugador)
    : QGraphicsPixmapItem()
{
    qDebug() << ">>> Creando Jugador" << numeroJugador;

    juegoPtr = juego;
    escenaPtr = escena;
    numJugador = numeroJugador;
    vidas = 5;

    // Ángulos iniciales diferentes para cada jugador
    if (numJugador == 1) {
        angulo = 45.0;  // Jugador 1 ahora apunta a la derecha (0° - 90°)
    } else {
        angulo = 135.0; // Jugador 2 ahora apunta a la izquierda (90° - 180°)
    }

    velocidad = 20.0;  // Velocidad inicial reducida de 50 a 20

    cargarAnimacion();

    setFlag(QGraphicsItem::ItemIsFocusable);

    // Jugador 1 a la IZQUIERDA, Jugador 2 a la DERECHA
    if (numJugador == 1) {
        setPos(50, juego->getSueloY() - 60);
    } else {
        setPos(escena->width() - 150, juego->getSueloY() - 60);
    }

    qDebug() << ">>> Jugador" << numeroJugador << "creado en posición:" << pos();
}

void Jugador::cargarAnimacion()
{
    baseCannonSprite.load(":/Recursos/Objetos/Canno.png");

    if (baseCannonSprite.isNull())
    {
        qDebug() << ">>> ERROR: No se pudo cargar Canno.png";
        baseCannonSprite = QPixmap(80, 60);
        baseCannonSprite.fill(numJugador == 1 ? QColor(0, 100, 200) : QColor(200, 0, 0));
    }

    qDebug() << ">>> Sprite base del cañón cargado:" << baseCannonSprite.width() << "x" << baseCannonSprite.height();

    actualizarFrame();
}

void Jugador::actualizarFrame()
{
    // Crear el pixmap rotado desde el sprite base
    QTransform transform;

    // Calcular el centro de rotación (base del cañón)
    QPointF center(baseCannonSprite.width() / 2.0, baseCannonSprite.height());

    // Mover el origen al centro de rotación
    transform.translate(center.x(), center.y());

    double anguloRotacion = 0.0;

    if (numJugador == 1) {
        // Jugador 1 (IZQUIERDA) debe apuntar DERECHA: 0° a 90°
        // El sprite apunta arriba (90°), queremos que apunte a la derecha
        // Para 45°: necesitamos rotar -45° desde arriba (90°)
        // anguloRotacion = 90 - angulo
        anguloRotacion = 90.0 - angulo;

        // Voltear horizontalmente para que mire a la derecha
        transform.scale(-1, 1);
    } else {
        // Jugador 2 (DERECHA) debe apuntar IZQUIERDA: 90° a 180°
        // El sprite apunta arriba (90°), queremos que apunte a la izquierda
        // Para 135°: necesitamos rotar 45° desde arriba (90°)
        // anguloRotacion = angulo - 90
        anguloRotacion = angulo - 90.0;
    }

    // Aplicar rotación
    transform.rotate(anguloRotacion);

    // Mover de vuelta
    transform.translate(-center.x(), -center.y());

    QPixmap rotatedPixmap = baseCannonSprite.transformed(transform, Qt::SmoothTransformation);

    if (!rotatedPixmap.isNull())
    {
        setPixmap(rotatedPixmap);

        // Centrar el pixmap rotado en la posición del jugador
        QPointF offset;
        offset.setX((baseCannonSprite.width() - rotatedPixmap.width()) / 2.0);
        offset.setY((baseCannonSprite.height() - rotatedPixmap.height()));
        setOffset(offset);

        if (scene()) {
            scene()->update();
        }
    }
    else
    {
        qDebug() << ">>> ERROR: El pixmap rotado es nulo!";
        setPixmap(baseCannonSprite);
    }
}

void Jugador::verificarColisionSuelo()
{
    if (!juegoPtr) return;

    qreal sueloY = juegoPtr->getSueloY();
    qreal alturaJugador = 60;

    if (y() + alturaJugador > sueloY) {
        setPos(x(), sueloY - alturaJugador);
    }
}

void Jugador::keyPressEvent(QKeyEvent *event)
{
    if (!event)
        return;

    qDebug() << ">>> Jugador" << numJugador << "keyPress:" << event->key();

    bool cambio = false;

    if (numJugador == 1) {
        // Jugador 1 (IZQUIERDA) apunta DERECHA: 0° a 90°
        if (event->key() == Qt::Key_Up) {
            angulo = qMin(90.0, angulo + 1.0);
            cambio = true;
        } else if (event->key() == Qt::Key_Down) {
            angulo = qMax(0.0, angulo - 1.0);
            cambio = true;
        }
    }
    else {
        // Jugador 2 (DERECHA) apunta IZQUIERDA: 90° a 180°
        if (event->key() == Qt::Key_Up) {
            angulo = qMin(180.0, angulo + 1.0);
            cambio = true;
        } else if (event->key() == Qt::Key_Down) {
            angulo = qMax(90.0, angulo - 1.0);
            cambio = true;
        }
    }

    // Controles de Velocidad (reducido el incremento de 2 a 1)
    if (event->key() == Qt::Key_Plus || event->key() == Qt::Key_Equal)
    {
        velocidad += 1;
        if (velocidad > 100)  // Reducido el máximo de 300 a 100
            velocidad = 100;
        cambio = true;
    }
    else if (event->key() == Qt::Key_Minus)
    {
        velocidad -= 1;
        if (velocidad < 5)  // Reducido el mínimo de 10 a 5
            velocidad = 5;
        cambio = true;
    }

    if (cambio) {
        actualizarFrame();
        if (juegoPtr)
            juegoPtr->actualizarPanel(angulo, velocidad);
    }

    if (event->key() == Qt::Key_Space)
    {
        qDebug() << ">>> Lanzar proyectil!";
        lanzar();
    }
}

void Jugador::lanzar()
{
    qDebug() << ">>> lanzar() llamado";

    if (!escenaPtr || !juegoPtr)
    {
        qDebug() << ">>> ERROR: Punteros nulos!";
        return;
    }

    // Calcular el punto de salida del proyectil (boca del cañón)
    double longitudCanon = baseCannonSprite.height() * 0.8;
    double angRad = qDegreesToRadians(angulo);

    // Posición base del cañón
    double baseX = x() + baseCannonSprite.width() / 2.0;
    double baseY = y() + baseCannonSprite.height();

    // Calcular la boca del cañón
    double bocaX = baseX + longitudCanon * qCos(angRad);
    double bocaY = baseY - longitudCanon * qSin(angRad);

    qDebug() << ">>> Creando proyectil en pos:" << bocaX << bocaY;
    Proyectil *p = new Proyectil(bocaX, bocaY, velocidad, angulo, numJugador);

    qDebug() << ">>> Conectando señales del proyectil...";
    QObject::connect(p, &Proyectil::impactoDetectado,
                     juegoPtr, &Juego::onImpactoProyectil);
    QObject::connect(p, &Proyectil::proyectilFinalizado,
                     juegoPtr, &Juego::onProyectilFinalizado);
    QObject::connect(p, &Proyectil::jugadorGolpeado,
                     juegoPtr, &Juego::onJugadorGolpeado);

    qDebug() << ">>> Agregando proyectil a la escena...";
    escenaPtr->addItem(p);

    qDebug() << ">>> Proyectil lanzado exitosamente";
}

void Jugador::perderVida()
{
    if (vidas > 0) {
        vidas--;
        qDebug() << ">>> Jugador" << numJugador << "pierde vida. Vidas restantes:" << vidas;
    }
}

void Jugador::resetear()
{
    qDebug() << ">>> Reseteando jugador" << numJugador;

    if (numJugador == 1) {
        angulo = 45.0;    // Jugador 1 apunta derecha
    } else {
        angulo = 135.0;   // Jugador 2 apunta izquierda
    }

    velocidad = 20.0;  // Velocidad inicial reducida
    vidas = 5;

    if (juegoPtr && escenaPtr) {
        if (numJugador == 1) {
            setPos(50, juegoPtr->getSueloY() - 60);
        } else {
            setPos(escenaPtr->width() - 150, juegoPtr->getSueloY() - 60);
        }
    }

    actualizarFrame();
}
