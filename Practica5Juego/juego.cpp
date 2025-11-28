#include "juego.h"
#include "jugador.h"
#include "proyectil.h"
#include "obstaculo.h"
#include <QHBoxLayout>
#include <QMessageBox>
#include <QDebug>
#include <QScreen>
#include <QGuiApplication>
#include <QTimer>

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

    // 1. Configurar la ventana principal y redimensionar según la pantalla
    setWindowTitle("Juego de Artilleria - 2 Jugadores");

    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect availableGeometry = screen->availableGeometry();
        int newWidth = availableGeometry.width();
        int newHeight = availableGeometry.height();

        const int MIN_WIDTH = 1000;
        const int MIN_HEIGHT = 650;

        if (newWidth < MIN_WIDTH) newWidth = MIN_WIDTH;
        if (newHeight < MIN_HEIGHT) newHeight = MIN_HEIGHT;

        resize(newWidth, newHeight);

        int x = availableGeometry.x() + (availableGeometry.width() - newWidth) / 2;
        int y = availableGeometry.y() + (availableGeometry.height() - newHeight) / 2;
        move(x, y);
    } else {
        qDebug() << "No se pudo obtener la pantalla principal. Usando tamaño por defecto.";
        resize(1000, 650);
    }

    // 2. Crear widget central
    widgetCentral = new QWidget(this);
    setCentralWidget(widgetCentral);

    // 3. Crear el panel de información PRIMERO para calcular su ancho
    qDebug() << "Creando panel de información...";
    labelJugador = new QLabel("Turno: Jugador 1");
    labelJugador->setStyleSheet(
        "font-weight: bold; "
        "font-size: 16px; "
        "color: #3498db; "
        "background: rgba(52, 152, 219, 0.1); "
        "border-radius: 5px; "
        "padding: 8px;"
        );
    labelJugador->setAlignment(Qt::AlignCenter);

    labelAngulo = new QLabel("Ángulo: 45°");
    labelAngulo->setStyleSheet("font-size: 13px; color: #ecf0f1;");

    labelVelocidad = new QLabel("Velocidad: 50");
    labelVelocidad->setStyleSheet("font-size: 13px; color: #ecf0f1;");

    labelPuntajeJ1 = new QLabel("🎮 Jugador 1: 0 pts");
    labelPuntajeJ1->setStyleSheet(
        "font-size: 13px; "
        "color: #2ecc71; "
        "font-weight: bold; "
        "background: rgba(46, 204, 113, 0.1); "
        "border-radius: 3px; "
        "padding: 5px;"
        );

    labelPuntajeJ2 = new QLabel("🎮 Jugador 2: 0 pts");
    labelPuntajeJ2->setStyleSheet(
        "font-size: 13px; "
        "color: #9b59b6; "
        "font-weight: bold; "
        "background: rgba(155, 89, 182, 0.1); "
        "border-radius: 3px; "
        "padding: 5px;"
        );

    labelFallosJ1 = new QLabel("❌ Fallos: 0/3");
    labelFallosJ1->setStyleSheet("font-size: 12px; color: #e67e22;");

    labelFallosJ2 = new QLabel("❌ Fallos: 0/3");
    labelFallosJ2->setStyleSheet("font-size: 12px; color: #e67e22;");

    btnReiniciar = new QPushButton("🔄 Reiniciar");
    connect(btnReiniciar, &QPushButton::clicked, this, &Juego::onReiniciar);

    QVBoxLayout *panel = new QVBoxLayout();
    panel->setSpacing(8);
    panel->setContentsMargins(10, 10, 10, 10);

    panel->addWidget(labelJugador);
    panel->addSpacing(15);

    // Sección de controles
    QLabel *labelControles = new QLabel("⚙️ CONTROLES");
    labelControles->setStyleSheet(
        "font-size: 12px; "
        "color: #95a5a6; "
        "font-weight: bold; "
        "padding: 3px;"
        );
    panel->addWidget(labelControles);
    panel->addWidget(labelAngulo);
    panel->addWidget(labelVelocidad);
    panel->addSpacing(15);

    // Sección de puntajes
    QLabel *labelPuntajes = new QLabel("🏆 PUNTAJES");
    labelPuntajes->setStyleSheet(
        "font-size: 12px; "
        "color: #95a5a6; "
        "font-weight: bold; "
        "padding: 3px;"
        );
    panel->addWidget(labelPuntajes);
    panel->addWidget(labelPuntajeJ1);
    panel->addWidget(labelFallosJ1);
    panel->addSpacing(8);
    panel->addWidget(labelPuntajeJ2);
    panel->addWidget(labelFallosJ2);
    panel->addSpacing(20);
    panel->addWidget(btnReiniciar);
    panel->addStretch();

    // Crear un widget contenedor para el panel con ancho fijo y estilo mejorado
    QWidget *panelWidget = new QWidget();
    panelWidget->setLayout(panel);
    panelWidget->setFixedWidth(200);
    panelWidget->setStyleSheet(
        "QWidget {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "       stop:0 #2c3e50, stop:1 #34495e);"
        "   padding: 15px;"
        "   border-left: 3px solid #3498db;"
        "}"
        "QLabel {"
        "   color: #ecf0f1;"
        "   font-size: 13px;"
        "   padding: 5px;"
        "   background: transparent;"
        "}"
        "QPushButton {"
        "   background-color: #e74c3c;"
        "   color: white;"
        "   border: none;"
        "   padding: 10px;"
        "   border-radius: 5px;"
        "   font-weight: bold;"
        "   font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #c0392b;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #a93226;"
        "}"
        );

    // 4. Calcular el tamaño de la escena basado en el tamaño de la ventana
    int anchoPanel = 180;
    int anchoEscena = width() - anchoPanel - 20; // 20 para márgenes
    int altoEscena = height() - 40; // 40 para márgenes superior e inferior

    qDebug() << "Creando escena con tamaño:" << anchoEscena << "x" << altoEscena;
    escena = new QGraphicsScene(0, 0, anchoEscena, altoEscena, this);
    escena->setBackgroundBrush(QBrush(QColor(135, 206, 235)));

    qDebug() << "Creando vista...";
    vista = new QGraphicsView(escena, widgetCentral);
    vista->setRenderHint(QPainter::Antialiasing);
    vista->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Calcular posición del suelo (90% de la altura de la escena)
    sueloY = altoEscena * 0.9;
    alturaSuelo = altoEscena - sueloY;

    qDebug() << "Creando jugadores...";
    // Crear jugadores posicionados sobre el suelo (no dentro del suelo)
    qreal alturaJugador = 30; // Altura aproximada del jugador

    qDebug() << "Creando jugador 1...";
    jugador1 = new Jugador(this, escena, 1);
    jugador1->setPos(50, sueloY - alturaJugador); // Restar la altura para que esté SOBRE el suelo
    escena->addItem(jugador1);

    qDebug() << "Creando jugador 2...";
    jugador2 = new Jugador(this, escena, 2);
    jugador2->setPos(50, sueloY - alturaJugador); // Restar la altura para que esté SOBRE el suelo
    jugador2->setVisible(false);
    escena->addItem(jugador2);

    jugadorActivo = jugador1;
    jugador1->setFocus();

    qDebug() << "Creando suelo...";
    // Dibujar suelo que ocupa todo el ancho de la escena
    QGraphicsRectItem *suelo = new QGraphicsRectItem(0, sueloY, anchoEscena, alturaSuelo);
    suelo->setBrush(QBrush(QColor(101, 67, 33)));
    escena->addItem(suelo);

    qDebug() << "Creando casa...";
    crearCasa();

    qDebug() << "Configurando layout...";
    // Layout general sin factores de estiramiento (el panel tiene ancho fijo)
    QHBoxLayout *layoutGeneral = new QHBoxLayout(widgetCentral);
    layoutGeneral->setContentsMargins(0, 0, 0, 0);
    layoutGeneral->setSpacing(0);

    layoutGeneral->addWidget(vista);
    layoutGeneral->addWidget(panelWidget);

    widgetCentral->setLayout(layoutGeneral);

    qDebug() << "Actualizando panel inicial...";
    actualizarPanel(45, 50);

    qDebug() << "=== JUEGO CONSTRUCTOR COMPLETADO ===";
}

void Juego::crearCasa()
{
    qDebug() << ">>> Creando casa con componentes de vida diferenciada";

    // Calcular posiciones relativas al tamaño de la escena
    qreal anchoEscena = escena->width();
    qreal altoEscena = escena->height();

    // La casa estará en el 65% del ancho de la escena
    qreal casaX = anchoEscena * 0.65;
    qreal casaAncho = 170;
    qreal casaAlto = 150;
    qreal casaY = sueloY - casaAlto;

    // Paredes laterales de la casa (5 hits cada una)
    Obstaculo *paredIzq = new Obstaculo(5);
    paredIzq->setRect(0, 0, 20, casaAlto);
    paredIzq->setPos(casaX, casaY);
    paredIzq->setBrush(QBrush(QColor(139, 69, 19)));
    escena->addItem(paredIzq);
    obstaculosCasa.append(paredIzq);

    Obstaculo *paredDer = new Obstaculo(5);
    paredDer->setRect(0, 0, 20, casaAlto);
    paredDer->setPos(casaX + casaAncho - 20, casaY);
    paredDer->setBrush(QBrush(QColor(139, 69, 19)));
    escena->addItem(paredDer);
    obstaculosCasa.append(paredDer);

    // Techo de la casa (10 hits cada pieza)
    Obstaculo *techoIzq = new Obstaculo(10);
    techoIzq->setRect(0, 0, 85, 20);
    techoIzq->setPos(casaX, casaY - 5);
    techoIzq->setBrush(QBrush(QColor(178, 34, 34)));
    escena->addItem(techoIzq);
    obstaculosCasa.append(techoIzq);

    Obstaculo *techoDer = new Obstaculo(10);
    techoDer->setRect(0, 0, 85, 20);
    techoDer->setPos(casaX + 85, casaY - 5);
    techoDer->setBrush(QBrush(QColor(178, 34, 34)));
    escena->addItem(techoDer);
    obstaculosCasa.append(techoDer);

    // Piso frontal de la casa (1 hit)
    Obstaculo *pisoFrontal = new Obstaculo(1);
    pisoFrontal->setRect(0, 0, casaAncho, 20);
    pisoFrontal->setPos(casaX, sueloY - 20);
    pisoFrontal->setBrush(QBrush(QColor(139, 69, 19)));
    escena->addItem(pisoFrontal);
    obstaculosCasa.append(pisoFrontal);

    // Puerta frontal (1 hit)
    Obstaculo *puerta = new Obstaculo(1);
    puerta->setRect(0, 0, 50, 80);
    puerta->setPos(casaX + 60, sueloY - 80);
    puerta->setBrush(QBrush(QColor(101, 67, 33)));
    escena->addItem(puerta);
    obstaculosCasa.append(puerta);

    // NPC dentro de la casa (1 hit)
    obstaculoNPC = new Obstaculo(1);
    obstaculoNPC->setRect(0, 0, 30, 50);
    obstaculoNPC->setPos(casaX + 70, casaY + 20);
    obstaculoNPC->setBrush(QBrush(QColor(255, 215, 0)));
    escena->addItem(obstaculoNPC);

    qDebug() << ">>> Casa creada: 2 paredes (5), 2 techos (10), 1 piso (1), 1 puerta (1), 1 NPC (1)";
}

QGraphicsScene* Juego::obtenerEscena() const
{
    return escena;
}

qreal Juego::getSueloY() const
{
    return sueloY;
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
                labelPuntajeJ1->setText(QString("🎮 Jugador 1: %1 pts").arg(puntajeJ1));
        }
        else
        {
            fallosJ1++;
            if (labelFallosJ1)
                labelFallosJ1->setText(QString("❌ Fallos: %1/3").arg(fallosJ1));

            if (fallosJ1 >= 3)
            {
                qDebug() << ">>> Jugador 1 llegó a 3 fallos, cambiando turno";
                QTimer::singleShot(500, this, [this]() {
                    cambiarTurno();
                    turnoEnProceso = false;
                });
                return;
            }
        }
    }
    else
    {
        if (acerto)
        {
            puntajeJ2 += 10;
            if (labelPuntajeJ2)
                labelPuntajeJ2->setText(QString("🎮 Jugador 2: %1 pts").arg(puntajeJ2));
        }
        else
        {
            fallosJ2++;
            if (labelFallosJ2)
                labelFallosJ2->setText(QString("❌ Fallos: %1/3").arg(fallosJ2));

            if (fallosJ2 >= 3)
            {
                qDebug() << ">>> Jugador 2 llegó a 3 fallos, cambiando turno";
                QTimer::singleShot(500, this, [this]() {
                    cambiarTurno();
                    turnoEnProceso = false;
                });
                return;
            }
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

    qDebug() << ">>> Recolectando proyectiles...";
    QList<QGraphicsItem*> todosLosItems = escena->items();
    QList<Proyectil*> proyectiles;

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
    for (int i = proyectiles.size() - 1; i >= 0; --i)
    {
        Proyectil *p = proyectiles[i];
        if (p && p->scene() == escena)
        {
            escena->removeItem(p);
            p->deleteLater();
        }
    }

    qDebug() << ">>> Eliminando obstáculos...";
    for (int i = obstaculosCasa.size() - 1; i >= 0; --i)
    {
        Obstaculo *obs = obstaculosCasa[i];
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
    puntajeJ1 = 0;
    puntajeJ2 = 0;
    fallosJ1 = 0;
    fallosJ2 = 0;
    jugadorActual = 1;
    turnoEnProceso = false;

    if (labelPuntajeJ1) labelPuntajeJ1->setText("Jugador 1: 0 puntos");
    if (labelPuntajeJ2) labelPuntajeJ2->setText("Jugador 2: 0 puntos");
    if (labelFallosJ1) labelFallosJ1->setText("Fallos J1: 0/3");
    if (labelFallosJ2) labelFallosJ2->setText("Fallos J2: 0/3");
    if (labelJugador) labelJugador->setText("Turno: Jugador 1");

    qDebug() << ">>> Recreando casa...";
    crearCasa();

    qDebug() << ">>> Reseteando jugadores...";
    if (jugador1) jugador1->resetear();
    if (jugador2) jugador2->resetear();
    if (jugador1) jugador1->setVisible(true);
    if (jugador2) jugador2->setVisible(false);
    jugadorActivo = jugador1;
    if (jugador1) jugador1->setFocus();

    actualizarPanel(45, 50);

    qDebug() << ">>> reiniciarJuego completado";
}
