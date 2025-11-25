#include "jugador.h"
#include "juego.h"
#include "proyectil.h"
#include <QtMath>
#include <QBrush>
#include <QGraphicsScene>
#include <QDebug>
#include <QPixmap>

Jugador::Jugador(Juego *juego, QGraphicsScene *escena, int numeroJugador)
    : QGraphicsPixmapItem(), frameAncho(0), frameAlto(0), framesPoraAngulo(4)
{
    qDebug() << ">>> Creando Jugador" << numeroJugador;

    juegoPtr = juego;
    escenaPtr = escena;
    numJugador = numeroJugador;

    angulo = 45.0;
    velocidad = 50.0;

    cargarAnimacion();

    setFlag(QGraphicsItem::ItemIsFocusable);
    setPos(numJugador == 1 ? 50 : 670, 520);

    qDebug() << ">>> Jugador" << numeroJugador << "constructor completado";
}

void Jugador::cargarAnimacion()
{
    // Cargar el sprite sheet
    spriteSheet.load(":/img/Tanques.png");

    if (spriteSheet.isNull())
    {
        qDebug() << ">>> ERROR: No se pudo cargar tanque.png";
        spriteSheet = QPixmap(320, 240);
        spriteSheet.fill(numJugador == 1 ? QColor(0, 100, 200) : QColor(200, 0, 0));
    }

    // Calcular tamaño de cada frame (asumiendo 4x4 = 16 frames)
    frameAncho = spriteSheet.width() / 4;
    frameAlto = spriteSheet.height() / 4;

    qDebug() << ">>> Sprite Sheet cargado:" << spriteSheet.width() << "x" << spriteSheet.height();
    qDebug() << ">>> Tamaño de frame:" << frameAncho << "x" << frameAlto;

    if (frameAncho <= 0 || frameAlto <= 0)
    {
        qDebug() << ">>> ERROR: Tamaño de frame inválido!";
        frameAncho = 80;
        frameAlto = 60;
    }

    actualizarFrame();
}

int Jugador::calcularFrameDelAngulo()
{
    // Mapear ángulo (0-90) a frame (0-3)
    // 0° = frame 0 (cañón arriba)
    // 30° = frame 1 (cañón medio-arriba)
    // 60° = frame 2 (cañón medio-abajo)
    // 90° = frame 3 (cañón abajo)
    int frame = static_cast<int>((angulo / 90.0) * 3.99);
    if (frame >= 4)
        frame = 3;
    return frame;
}

void Jugador::actualizarFrame()
{
    int frameIndex = calcularFrameDelAngulo();

    // Invertir frameIndex para que 0° sea frame 3 (cañón arriba)
    frameIndex = 3 - frameIndex;

    // Seleccionar fila según jugador (filas 0,1 para jugador 1; filas 2,3 para jugador 2)
    // Primera dirección: fila par (0,2)
    // Segunda dirección: fila impar (1,3)
    int fila = (numJugador == 1) ? 0 : 2;
    int columna = frameIndex;

    // Extraer el frame del sprite sheet
    QRect rectFrame(columna * frameAncho, fila * frameAlto, frameAncho, frameAlto);

    qDebug() << ">>> Extrayendo frame - Rect:" << rectFrame;
    qDebug() << ">>> SpriteSheet null?" << spriteSheet.isNull() << "SpriteSheet size:" << spriteSheet.size();

    QPixmap frame = spriteSheet.copy(rectFrame);

    if (!frame.isNull())
    {
        qDebug() << ">>> Frame extraído correctamente:" << frame.width() << "x" << frame.height();

        // Escalar el frame a un tamaño visible (80x60)
        QPixmap frameEscalado = frame.scaledToWidth(80, Qt::SmoothTransformation);

        qDebug() << ">>> Frame escalado a:" << frameEscalado.width() << "x" << frameEscalado.height();

        setPixmap(frameEscalado);
    }
    else
    {
        qDebug() << ">>> ERROR: Frame es nulo!";
        // Mostrar el sprite sheet completo como fallback
        setPixmap(spriteSheet);
    }

    qDebug() << ">>> Ángulo:" << angulo << "Frame:" << frameIndex << "Fila:" << fila << "Columna:" << columna;
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
        actualizarFrame();
        if (juegoPtr)
            juegoPtr->actualizarPanel(angulo, velocidad);
    }
    else if (event->key() == Qt::Key_Down)
    {
        angulo -= 1;
        if (angulo < 0)
            angulo = 0;
        actualizarFrame();
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
    setPos(numJugador == 10 ? 500 : 670, 520);
    actualizarFrame();
}
