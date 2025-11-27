#include "juego.h"
#include "jugador.h"
#include "proyectil.h"
#include "obstaculo.h"
#include <QHBoxLayout>
#include <QMessageBox>
#include <QDebug>

Juego::Juego(QWidget *parent)
    : QMainWindow(parent)
    , widgetCentral(nullptr)
    , jugadorActual(1)
    , puntajeJ1(0)
    , puntajeJ2(0)
    , fallosJ1(0)
    , fallosJ2(0)
    , turnoEnProceso(false)
{
    qDebug() << "=== JUEGO CONSTRUCTOR INICIADO ===";

    // Configurar la ventana principal
    setWindowTitle("Juego de Artilleria - 2 Jugadores");
    resize(1000, 650);

    // Crear widget central
    widgetCentral = new QWidget(this);
    setCentralWidget(widgetCentral);

    qDebug() << "Creando escena...";
    escena = new QGraphicsScene(0, 0, 800, 600, this);
    escena->setBackgroundBrush(QBrush(QColor(135, 206, 235)));

    qDebug() << "Creando vista...";
    vista = new QGraphicsView(escena, widgetCentral);
    vista->setRenderHint(QPainter::Antialiasing);
    vista->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setFixedSize(800, 600);

    qDebug() << "Creando jugadores...";
    // Crear jugadores
    qDebug() << "Creando jugador 1...";
    jugador1 = new Jugador(this, escena, 1);
    qDebug() << "Posicionando jugador 1...";
    jugador1->setPos(50, 520);
    qDebug() << "Agregando jugador 1 a escena...";
    escena->addItem(jugador1);

    qDebug() << "Creando jugador 2...";
    jugador2 = new Jugador(this, escena, 2);
    qDebug() << "Posicionando jugador 2...";
    jugador2->setPos(50, 520);
    qDebug() << "Ocultando jugador 2...";
    jugador2->setVisible(false);
    qDebug() << "Agregando jugador 2 a escena...";
    escena->addItem(jugador2);

    qDebug() << "Configurando jugador activo...";
    jugadorActivo = jugador1;
    qDebug() << "Dando foco a jugador 1...";
    jugador1->setFocus();

    qDebug() << "Creando suelo...";
    // Dibujar suelo
    QGraphicsRectItem *suelo = new QGraphicsRectItem(0, 550, 800, 50);
    suelo->setBrush(QBrush(QColor(101, 67, 33)));
    escena->addItem(suelo);

    qDebug() << "Creando casa...";
    // Crear la casa con obstáculos
    crearCasa();

    qDebug() << "Creando panel de información...";
    // Panel de información
    labelJugador = new QLabel("Turno: Jugador 1");
    labelJugador->setStyleSheet("font-weight: bold; font-size: 14px;");

    labelAngulo = new QLabel("Angulo: 45°");
    labelVelocidad = new QLabel("Velocidad: 50");

    labelPuntajeJ1 = new QLabel("Jugador 1: 0 puntos");
    labelPuntajeJ2 = new QLabel("Jugador 2: 0 puntos");

    labelFallosJ1 = new QLabel("Fallos J1: 0/3");
    labelFallosJ2 = new QLabel("Fallos J2: 0/3");

    btnReiniciar = new QPushButton("Reiniciar Juego");
    connect(btnReiniciar, &QPushButton::clicked, this, &Juego::onReiniciar);

    QVBoxLayout *panel = new QVBoxLayout();
    panel->addWidget(labelJugador);
    panel->addSpacing(10);
    panel->addWidget(labelAngulo);
    panel->addWidget(labelVelocidad);
    panel->addSpacing(20);
    panel->addWidget(labelPuntajeJ1);
    panel->addWidget(labelFallosJ1);
    panel->addSpacing(10);
    panel->addWidget(labelPuntajeJ2);
    panel->addWidget(labelFallosJ2);
    panel->addSpacing(20);
    panel->addWidget(btnReiniciar);
    panel->addStretch();

    qDebug() << "Configurando layout...";
    // Layout general
    QHBoxLayout *layoutGeneral = new QHBoxLayout(widgetCentral);
    layoutGeneral->addWidget(vista);
    layoutGeneral->addLayout(panel);

    widgetCentral->setLayout(layoutGeneral);

    qDebug() << "Actualizando panel inicial...";
    actualizarPanel(45, 50);

    qDebug() << "=== JUEGO CONSTRUCTOR COMPLETADO ===";
}

void Juego::crearCasa()
{
    // Paredes laterales de la casa
    Obstaculo *paredIzq = new Obstaculo(80);
    paredIzq->setRect(0, 0, 20, 150);
    paredIzq->setPos(500, 400);
    paredIzq->setBrush(QBrush(QColor(139, 69, 19)));
    escena->addItem(paredIzq);
    obstaculosCasa.append(paredIzq);

    Obstaculo *paredDer = new Obstaculo(80);
    paredDer->setRect(0, 0, 20, 150);
    paredDer->setPos(650, 400);
    paredDer->setBrush(QBrush(QColor(139, 69, 19)));
    escena->addItem(paredDer);
    obstaculosCasa.append(paredDer);

    // Techo de la casa
    Obstaculo *techoIzq = new Obstaculo(60);
    techoIzq->setRect(0, 0, 80, 15);
    techoIzq->setPos(490, 385);
    techoIzq->setRotation(-30);
    techoIzq->setBrush(QBrush(QColor(178, 34, 34)));
    escena->addItem(techoIzq);
    obstaculosCasa.append(techoIzq);

    Obstaculo *techoDer = new Obstaculo(60);
    techoDer->setRect(0, 0, 80, 15);
    techoDer->setPos(590, 385);
    techoDer->setRotation(30);
    techoDer->setBrush(QBrush(QColor(178, 34, 34)));
    escena->addItem(techoDer);
    obstaculosCasa.append(techoDer);

    // Puerta frontal
    Obstaculo *puerta = new Obstaculo(50);
    puerta->setRect(0, 0, 40, 80);
    puerta->setPos(565, 470);
    puerta->setBrush(QBrush(QColor(101, 67, 33)));
    escena->addItem(puerta);
    obstaculosCasa.append(puerta);

    // NPC dentro de la casa
    obstaculoNPC = new Obstaculo(100);
    obstaculoNPC->setRect(0, 0, 30, 50);
    obstaculoNPC->setPos(570, 500);
    obstaculoNPC->setBrush(QBrush(QColor(255, 215, 0)));
    escena->addItem(obstaculoNPC);
}

QGraphicsScene* Juego::obtenerEscena() const
{
    return escena;
}

void Juego::actualizarPanel(double angulo, double velocidad)
{
    if (labelAngulo && labelVelocidad)
    {
        labelAngulo->setText(QString("Angulo: %1°").arg(angulo, 0, 'f', 1));
        labelVelocidad->setText(QString("Velocidad: %1").arg(velocidad, 0, 'f', 1));
    }
}

void Juego::onImpactoProyectil(bool acerto)
{
    qDebug() << ">>> onImpactoProyectil llamado, acerto:" << acerto << "turnoEnProceso:" << turnoEnProceso;

    if (turnoEnProceso)
    {
        qDebug() << ">>> Turno ya en proceso, ignorando";
        return;
    }

    turnoEnProceso = true;
    registrarImpacto(acerto);
}

void Juego::onProyectilFinalizado()
{
    qDebug() << ">>> onProyectilFinalizado llamado, turnoEnProceso:" << turnoEnProceso;

    if (!turnoEnProceso)
    {
        qDebug() << ">>> Turno no estaba en proceso, ignorando";
        return;
    }

    // Usar QTimer para cambiar turno después de que el proyectil se haya destruido completamente
    QTimer::singleShot(50, this, [this]() {
        qDebug() << ">>> Cambiando turno después de delay";
        cambiarTurno();
        turnoEnProceso = false;
    });
}

void Juego::registrarImpacto(bool acerto)
{
    qDebug() << ">>> registrarImpacto, jugador:" << jugadorActual << "acerto:" << acerto;

    if (jugadorActual == 1)
    {
        if (acerto)
        {
            puntajeJ1 += 10;
            if (labelPuntajeJ1)
                labelPuntajeJ1->setText(QString("Jugador 1: %1 puntos").arg(puntajeJ1));
        }
        else
        {
            fallosJ1++;
            if (labelFallosJ1)
                labelFallosJ1->setText(QString("Fallos J1: %1/3").arg(fallosJ1));
        }
    }
    else
    {
        if (acerto)
        {
            puntajeJ2 += 10;
            if (labelPuntajeJ2)
                labelPuntajeJ2->setText(QString("Jugador 2: %1 puntos").arg(puntajeJ2));
        }
        else
        {
            fallosJ2++;
            if (labelFallosJ2)
                labelFallosJ2->setText(QString("Fallos J2: %1/3").arg(fallosJ2));
        }
    }

    verificarFinJuego();
}

void Juego::cambiarTurno()
{
    qDebug() << ">>> cambiarTurno, jugadorActual:" << jugadorActual;

    if (!jugador1 || !jugador2)
    {
        qDebug() << ">>> ERROR: Jugadores son nullptr!";
        return;
    }

    if (jugadorActual == 1)
    {
        jugadorActual = 2;
        jugador1->setVisible(false);
        jugador2->setVisible(true);
        jugadorActivo = jugador2;
        jugador2->setFocus();
        if (labelJugador)
            labelJugador->setText("Turno: Jugador 2");
    }
    else
    {
        jugadorActual = 1;
        jugador2->setVisible(false);
        jugador1->setVisible(true);
        jugadorActivo = jugador1;
        jugador1->setFocus();
        if (labelJugador)
            labelJugador->setText("Turno: Jugador 1");
    }

    if (jugadorActivo)
        actualizarPanel(jugadorActivo->getAngulo(), jugadorActivo->getVelocidad());
}

void Juego::verificarFinJuego()
{
    if (fallosJ1 >= 3 || fallosJ2 >= 3 || (obstaculoNPC && obstaculoNPC->obtenerVida() <= 0))
    {
        qDebug() << ">>> Fin del juego detectado";
        mostrarGanador();
    }
}

void Juego::mostrarGanador()
{
    QString mensaje;

    if (fallosJ1 >= 3 && fallosJ2 >= 3)
    {
        mensaje = "¡Empate! Ambos jugadores fallaron 3 veces.";
    }
    else if (fallosJ1 >= 3)
    {
        mensaje = QString("¡Jugador 2 gana!\nJugador 1 fallo 3 veces.\nPuntaje final:\nJugador 1: %1\nJugador 2: %2")
                      .arg(puntajeJ1).arg(puntajeJ2);
    }
    else if (fallosJ2 >= 3)
    {
        mensaje = QString("¡Jugador 1 gana!\nJugador 2 fallo 3 veces.\nPuntaje final:\nJugador 1: %1\nJugador 2: %2")
                      .arg(puntajeJ1).arg(puntajeJ2);
    }
    else if (puntajeJ1 > puntajeJ2)
    {
        mensaje = QString("¡Jugador 1 gana!\n¡El NPC fue derrotado!\nPuntaje final:\nJugador 1: %1\nJugador 2: %2")
                      .arg(puntajeJ1).arg(puntajeJ2);
    }
    else if (puntajeJ2 > puntajeJ1)
    {
        mensaje = QString("¡Jugador 2 gana!\n¡El NPC fue derrotado!\nPuntaje final:\nJugador 1: %1\nJugador 2: %2")
                      .arg(puntajeJ1).arg(puntajeJ2);
    }
    else
    {
        mensaje = QString("¡Empate!\n¡El NPC fue derrotado!\nPuntaje final: %1 - %1")
                      .arg(puntajeJ1);
    }

    QMessageBox::information(this, "Fin del Juego", mensaje);
}

void Juego::onReiniciar()
{
    qDebug() << ">>> Reiniciar juego solicitado";
    reiniciarJuego();
}

void Juego::reiniciarJuego()
{
    qDebug() << ">>> reiniciarJuego iniciado";

    if (!escena)
    {
        qDebug() << ">>> ERROR: escena es nullptr!";
        return;
    }

    // Limpiar todos los proyectiles de manera segura
    QList<QGraphicsItem*> todosLosItems = escena->items();
    QList<Proyectil*> proyectiles;

    qDebug() << ">>> Recolectando proyectiles...";
    // Primero recolectar todos los proyectiles
    for (QGraphicsItem *item : todosLosItems)
    {
        if (!item)
            continue;

        Proyectil *p = dynamic_cast<Proyectil*>(item);
        if (p)
        {
            proyectiles.append(p);
        }
    }

    qDebug() << ">>> Eliminando" << proyectiles.size() << "proyectiles...";
    // Luego eliminarlos de forma segura
    for (Proyectil *p : proyectiles)
    {
        if (p && p->scene() == escena)
        {
            escena->removeItem(p);
            p->deleteLater();
        }
    }

    qDebug() << ">>> Eliminando obstáculos...";
    // Limpiar obstáculos viejos
    for (Obstaculo *obs : obstaculosCasa)
    {
        if (obs && obs->scene() == escena)
        {
            escena->removeItem(obs);
            delete obs;
        }
    }
    obstaculosCasa.clear();

    if (obstaculoNPC)
    {
        if (obstaculoNPC->scene() == escena)
            escena->removeItem(obstaculoNPC);
        delete obstaculoNPC;
        obstaculoNPC = nullptr;
    }

    qDebug() << ">>> Reiniciando valores...";
    // Reiniciar valores
    puntajeJ1 = 0;
    puntajeJ2 = 0;
    fallosJ1 = 0;
    fallosJ2 = 0;
    jugadorActual = 1;
    turnoEnProceso = false;

    // Actualizar labels
    if (labelPuntajeJ1) labelPuntajeJ1->setText("Jugador 1: 0 puntos");
    if (labelPuntajeJ2) labelPuntajeJ2->setText("Jugador 2: 0 puntos");
    if (labelFallosJ1) labelFallosJ1->setText("Fallos J1: 0/3");
    if (labelFallosJ2) labelFallosJ2->setText("Fallos J2: 0/3");
    if (labelJugador) labelJugador->setText("Turno: Jugador 1");

    qDebug() << ">>> Recreando casa...";
    // Recrear casa
    crearCasa();

    qDebug() << ">>> Reseteando jugadores...";
    // Resetear jugadores
    if (jugador1) jugador1->resetear();
    if (jugador2) jugador2->resetear();
    if (jugador1) jugador1->setVisible(true);
    if (jugador2) jugador2->setVisible(false);
    jugadorActivo = jugador1;
    if (jugador1) jugador1->setFocus();

    actualizarPanel(45, 50);

    qDebug() << ">>> reiniciarJuego completado";
}
