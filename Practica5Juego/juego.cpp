#include "juego.h"
#include "jugador.h"
#include "proyectil.h"
#include "obstaculo.h"
#include <QHBoxLayout>
#include <QDebug>
#include <QScreen>
#include <QGuiApplication>
#include <QTimer>
#include <QPainter>
#include <QMessageBox> // Mantener por si se requiere para errores, pero no para fin de juego.
// [CORREGIDO] Se eliminó el 'void' ya que los constructores no tienen tipo de retorno.
Juego::Juego(QWidget *parent)
    : QMainWindow(parent)
    , widgetCentral(nullptr)
    , jugadorActual(1)
    , puntajeJ1(0)
    , puntajeJ2(0)
    , turnoEnProceso(false)
    , hitOccurredThisTurn(false)
    , proyectilActivo(false)
    , finJuegoOverlay(nullptr)
    , fondoGameOver(nullptr)
{
    qDebug() << "=== JUEGO CONSTRUCTOR INICIADO ===";

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
        resize(1000, 650);
    }

    widgetCentral = new QWidget(this);
    setCentralWidget(widgetCentral);

    // Crear panel de información
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

    labelVelocidad = new QLabel("Velocidad: 20");
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

    // [CORREGIDO] Cálculo de la escena y suelo
    // Calcular tamaño de la escena
    int anchoPanel = 180;
    int anchoEscena = width() - anchoPanel - 20;
    int altoEscena = height() - 40;

    labelVidasJ1 = new QLabel("❤️ Vidas: 5");
    labelVidasJ1->setStyleSheet("font-size: 12px; color: #e74c3c; font-weight: bold;");

    labelVidasJ2 = new QLabel("❤️ Vidas: 5");
    labelVidasJ2->setStyleSheet("font-size: 12px; color: #e74c3c; font-weight: bold;");

    btnReiniciar = new QPushButton("🔄 Reiniciar");
    connect(btnReiniciar, &QPushButton::clicked, this, &Juego::onReiniciar);

    QVBoxLayout *panel = new QVBoxLayout();
    panel->setSpacing(8);
    panel->setContentsMargins(10, 10, 10, 10);

    panel->addWidget(labelJugador);
    panel->addSpacing(15);

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

    QLabel *labelPuntajes = new QLabel("🏆 PUNTAJES Y VIDAS");
    labelPuntajes->setStyleSheet(
        "font-size: 12px; "
        "color: #95a5a6; "
        "font-weight: bold; "
        "padding: 3px;"
        );
    panel->addWidget(labelPuntajes);
    panel->addWidget(labelPuntajeJ1);
    panel->addWidget(labelVidasJ1);
    panel->addSpacing(8);
    panel->addWidget(labelPuntajeJ2);
    panel->addWidget(labelVidasJ2);
    panel->addSpacing(20);
    panel->addWidget(btnReiniciar);
    panel->addStretch();

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

    qDebug() << "Creando escena con tamaño:" << anchoEscena << "x" << altoEscena;
    escena = new QGraphicsScene(0, 0, anchoEscena, altoEscena, this);
    escena->setBackgroundBrush(QBrush(QPixmap(":/Recursos/Fondos/Background.jpg"))); // Fondo inicial

    qDebug() << "Creando vista...";
    vista = new QGraphicsView(escena, widgetCentral);
    vista->setRenderHint(QPainter::Antialiasing);
    vista->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // [NUEVO] Crear el QGraphicsPixmapItem para el fondo de Game Over.
    QPixmap pixmapGameOver(":/Recursos/Fondos/GameOver.jpg"); // Asume que tienes este recurso
    if (!pixmapGameOver.isNull()) {
        fondoGameOver = new QGraphicsPixmapItem(pixmapGameOver.scaled(anchoEscena, altoEscena, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        fondoGameOver->setPos(0, 0);
        fondoGameOver->setZValue(-1); // Muy bajo, para que esté debajo de todo
        escena->addItem(fondoGameOver);
        fondoGameOver->setVisible(false);
    }

    sueloY = altoEscena * 0.9;
    alturaSuelo = altoEscena - sueloY;

    qDebug() << "Creando jugadores...";

    qDebug() << "Creando jugador 1...";
    jugador1 = new Jugador(this, escena, 1);
    escena->addItem(jugador1);

    qDebug() << "Creando jugador 2...";
    jugador2 = new Jugador(this, escena, 2);
    escena->addItem(jugador2);

    jugadorActivo = jugador1;
    jugador1->setFocus();

    qDebug() << "Creando suelo...";
    QGraphicsRectItem *suelo = new QGraphicsRectItem(0, sueloY, anchoEscena, alturaSuelo);
    suelo->setBrush(QBrush(QColor(101, 67, 33)));
    escena->addItem(suelo);

    // [CORREGIDO] Crear las casas después de inicializar escena y sueloY
    qDebug() << "Creando casas y targets...";
    crearCasa(escena->width() * 0.25 - 85, 170, 150); // Casa 1 (Izquierda)
    crearCasa(escena->width() * 0.75 - 85, 170, 150); // Casa 2 (Derecha)

    // Removida la llamada a crearCasa() sin argumentos

    qDebug() << "Creando corazones...";
    crearCorazones();

    // [NUEVO] Crear e inicializar el overlay de fin de juego (oculto por defecto)
    finJuegoOverlay = new FinDeJuegoOverlay(anchoEscena, altoEscena);
    escena->addItem(finJuegoOverlay);

    // Conectar la señal del botón del overlay al slot de reinicio dedicado
    connect(finJuegoOverlay, &FinDeJuegoOverlay::reiniciarJuegoSolicitado,
            this, &Juego::onReiniciarDesdeOverlay);


    qDebug() << "Configurando layout...";
    QHBoxLayout *layoutGeneral = new QHBoxLayout(widgetCentral);
    layoutGeneral->setContentsMargins(0, 0, 0, 0);
    layoutGeneral->setSpacing(0);

    layoutGeneral->addWidget(vista);
    layoutGeneral->addWidget(panelWidget);

    widgetCentral->setLayout(layoutGeneral);

    qDebug() << "Actualizando panel inicial...";
    actualizarPanel(45, 20);

    qDebug() << "=== JUEGO CONSTRUCTOR COMPLETADO ===";
}

void Juego::crearCorazones()
{
    qDebug() << ">>> Creando corazones visuales";

    // Crear pixmap de corazón más grande con borde negro
    QPixmap corazon(40, 40);
    corazon.fill(Qt::transparent);

    QPainter painter(&corazon);
    painter.setRenderHint(QPainter::Antialiasing);

    // Dibujar forma de corazón
    QPainterPath path;
    path.moveTo(20, 35);

    // Lado izquierdo del corazón
    path.cubicTo(5, 25, 5, 15, 10, 10);
    path.cubicTo(15, 5, 20, 10, 20, 15);

    // Lado derecho del corazón
    path.cubicTo(20, 10, 25, 5, 30, 10);
    path.cubicTo(35, 15, 35, 25, 20, 35);

    // Rellenar con rojo
    painter.setBrush(QBrush(QColor(231, 76, 60)));
    painter.setPen(Qt::NoPen);
    painter.drawPath(path);

    // Dibujar borde negro
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(Qt::black, 3));
    painter.drawPath(path);

    painter.end();

    // Corazones para Jugador 1 (arriba a la derecha)
    for (int i = 0; i < 5; i++) {
        QGraphicsPixmapItem *corazonItem = new QGraphicsPixmapItem(corazon);
        corazonItem->setPos(escena->width() - 240 + (i * 45), 10);
        corazonItem->setZValue(100);
        escena->addItem(corazonItem);
        corazonesJ1.append(corazonItem);
    }

    // Corazones para Jugador 2 (arriba a la izquierda)
    for (int i = 0; i < 5; i++) {
        QGraphicsPixmapItem *corazonItem = new QGraphicsPixmapItem(corazon);
        corazonItem->setPos(10 + (i * 45), 10);
        corazonItem->setZValue(100);
        escena->addItem(corazonItem);
        corazonesJ2.append(corazonItem);
    }

    qDebug() << ">>> Corazones creados";
}

void Juego::actualizarCorazones()
{
    // Actualizar corazones de Jugador 1
    int vidasJ1 = jugador1 ? jugador1->getVidas() : 0;
    for (int i = 0; i < corazonesJ1.size(); i++) {
        if (i < vidasJ1) {
            corazonesJ1[i]->setOpacity(1.0);
        } else {
            corazonesJ1[i]->setOpacity(0.2);
        }
    }

    // Actualizar corazones de Jugador 2
    int vidasJ2 = jugador2 ? jugador2->getVidas() : 0;
    for (int i = 0; i < corazonesJ2.size(); i++) {
        if (i < vidasJ2) {
            corazonesJ2[i]->setOpacity(1.0);
        } else {
            corazonesJ2[i]->setOpacity(0.2);
        }
    }

    // Actualizar labels
    if (labelVidasJ1) labelVidasJ1->setText(QString("❤️ Vidas: %1").arg(vidasJ1));
    if (labelVidasJ2) labelVidasJ2->setText(QString("❤️ Vidas: %1").arg(vidasJ2));
}

void Juego::crearCasa(qreal posX, qreal ancho, qreal alto)
{
    qDebug() << ">>> Creando casa en X:" << posX;

    qreal casaX = posX;
    qreal casaAncho = ancho;
    qreal casaAlto = alto;
    qreal casaY = sueloY - casaAlto;

    // Bloques de la casa (Obstáculos)
    // Pared Izquierda (Vida 20 - AUMENTADA)
    Obstaculo *paredIzq = new Obstaculo(20);
    paredIzq->setRect(0, 0, 20, casaAlto);
    paredIzq->setPos(casaX, casaY);
    paredIzq->setBrush(QBrush(QColor(139, 69, 19)));
    obstaculosCasa.append(paredIzq);
    escena->addItem(paredIzq);

    // Pared Derecha (Vida 20 - AUMENTADA)
    Obstaculo *paredDer = new Obstaculo(20);
    paredDer->setRect(0, 0, 20, casaAlto);
    paredDer->setPos(casaX + casaAncho - 20, casaY);
    paredDer->setBrush(QBrush(QColor(139, 69, 19)));
    obstaculosCasa.append(paredDer);
    escena->addItem(paredDer);

    // Techo (Vida 40 - AUMENTADA)
    Obstaculo *techo = new Obstaculo(40);
    techo->setRect(0, 0, casaAncho, 20);
    techo->setPos(casaX, casaY - 5);
    techo->setBrush(QBrush(QColor(178, 34, 34)));
    obstaculosCasa.append(techo);
    escena->addItem(techo);

    // [CORREGIDO] Creación del TARGET (ex-NPC) como un Obstaculo especial.
    // Vida aumentada a 10 para el target.
    Obstaculo *target = new Obstaculo(10); // Target con 10 de vida
    target->setRect(0, 0, 30, 30); // Tamaño del target
    target->setPos(casaX + (casaAncho / 2) - 15, casaY + casaAlto - 50); // Posición centrada
    target->setBrush(QBrush(QColor(255, 215, 0))); // Color dorado para diferenciar
    targets.append(target); // Agregar a la nueva lista de targets
    obstaculosCasa.append(target); // También añadir a la lista de obstáculos
    escena->addItem(target);

    qDebug() << ">>> Casa y Target creados en" << posX;
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


void Juego::onJugadorGolpeado(int jugadorNumero)
{
    qDebug() << ">>> Jugador" << jugadorNumero << "fue golpeado!";

    // Determinar qué jugador fue golpeado
    Jugador *jugadorGolpeado = (jugadorNumero == 1) ? jugador1 : jugador2;

    if (jugadorGolpeado) {
        jugadorGolpeado->perderVida();
        actualizarCorazones();

        // Si golpea al jugador, se considera un acierto.
        hitOccurredThisTurn = true;

        if (jugadorGolpeado->getVidas() <= 0) {
            qDebug() << ">>> Jugador" << jugadorNumero << "perdió todas las vidas!";
            verificarFinJuego();
        }
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

    // Si acierta a target/obstáculo, marca el turno como un acierto.
    if (acerto) {
        hitOccurredThisTurn = true;
    }

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

    // LÓGICA DE FALLO: Si no hubo acierto (ni a jugador, ni a target, ni a obstáculo), se considera fallo.
    if (!hitOccurredThisTurn) {
        qDebug() << ">>> FALLO detectado. Jugador" << jugadorActual << "pierde una vida.";
        Jugador *jugadorPerdedor = (jugadorActual == 1) ? jugador1 : jugador2;

        if (jugadorPerdedor) {
            jugadorPerdedor->perderVida();
            actualizarCorazones();
        }
        verificarFinJuego();
    }

    QTimer::singleShot(50, this, [this]() {
        qDebug() << ">>> Cambiando turno después de delay";
        cambiarTurno();
        turnoEnProceso = false;
        proyectilActivo = false; // Permite el siguiente lanzamiento
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
    }
    else
    {
        if (acerto)
        {
            puntajeJ2 += 10;
            if (labelPuntajeJ2)
                labelPuntajeJ2->setText(QString("🎮 Jugador 2: %1 pts").arg(puntajeJ2));
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
        jugadorActivo = jugador2;
        jugador2->setFocus();
        if (labelJugador)
            labelJugador->setText("Turno: Jugador 2");
    }
    else
    {
        jugadorActual = 1;
        jugadorActivo = jugador1;
        jugador1->setFocus();
        if (labelJugador)
            labelJugador->setText("Turno: Jugador 1");
    }

    if (jugadorActivo)
    {
        actualizarPanel(jugadorActivo->getAngulo(), jugadorActivo->getVelocidad());
    }

    // Resetear el flag de acierto para el siguiente turno
    hitOccurredThisTurn = false;
}

// Función para manejar el lanzamiento y su restricción
void Juego::lanzarProyectil(double angulo, double velocidad, qreal x, qreal y, int numJugador)
{
    // Restricción de un solo proyectil activo
    if (proyectilActivo) {
        qDebug() << ">>> Lanzamiento denegado: Ya hay un proyectil activo en la escena.";
        return;
    }

    proyectilActivo = true; // Marcar el proyectil como activo

    qDebug() << ">>> Lanzando proyectil para Jugador" << numJugador;
    Proyectil *p = new Proyectil(x, y, velocidad, angulo, numJugador);

    // Conectar señales
    QObject::connect(p, &Proyectil::impactoDetectado,
                     this, &Juego::onImpactoProyectil);
    QObject::connect(p, &Proyectil::proyectilFinalizado,
                     this, &Juego::onProyectilFinalizado);
    QObject::connect(p, &Proyectil::jugadorGolpeado,
                     this, &Juego::onJugadorGolpeado);

    escena->addItem(p);

    qDebug() << ">>> Proyectil lanzado exitosamente";

}

void Juego::verificarFinJuego()
{
    bool j1SinVidas = jugador1 && jugador1->getVidas() <= 0;
    bool j2SinVidas = jugador2 && jugador2->getVidas() <= 0;

    // [CORREGIDO] Verificar si TODOS los targets (ex-NPCs) han sido destruidos.
    bool todosTargetsDestruidos = true;
    for (Obstaculo *target : targets) {
        if (target->obtenerVida() > 0) {
            todosTargetsDestruidos = false;
            break;
        }
    }

    if (j1SinVidas || j2SinVidas || todosTargetsDestruidos)
    {
        qDebug() << ">>> Fin del juego detectado";
        mostrarGanador();
    }
}

// [NUEVO] Gestiona el cambio de fondo de escena y visibilidad de elementos
void Juego::cambiarFondoEscena(bool aGameOver)
{
    // Ocultar proyectiles si están activos
    QList<QGraphicsItem*> items = escena->items();
    QList<Proyectil*> proyectiles;
    for (QGraphicsItem *item : items) {
        if (dynamic_cast<Proyectil*>(item)) {
            item->setVisible(!aGameOver);
            proyectiles.append(dynamic_cast<Proyectil*>(item));
        }
    }

    if (aGameOver) {
        // Modo Game Over: Oculta fondo normal, muestra fondo de Game Over
        escena->setBackgroundBrush(Qt::NoBrush);
        if (fondoGameOver) {
            fondoGameOver->setVisible(true);
        }

        // Ocultar elementos del juego
        if (jugador1) jugador1->setVisible(false);
        if (jugador2) jugador2->setVisible(false);

        for (QGraphicsPixmapItem *c : corazonesJ1) c->setVisible(false);
        for (QGraphicsPixmapItem *c : corazonesJ2) c->setVisible(false);

        // Ocultar obstaculos
        for (Obstaculo *o : obstaculosCasa) o->setVisible(false);
        if (obstaculoNPC) obstaculoNPC->setVisible(false);

        // Detener proyectiles si no están destruidos
        for (Proyectil *p : proyectiles) {
            // Asumiendo que Proyectil tiene un método para detener su timer
            // Si no lo tiene, es un buen lugar para añadirlo o dejar que terminen.
        }

    } else {
        // Restaurar modo normal
        escena->setBackgroundBrush(QBrush(QPixmap(":/Recursos/Fondos/Background.jpg")));
        if (fondoGameOver) {
            fondoGameOver->setVisible(false);
        }

        // Mostrar elementos del juego
        if (jugador1) jugador1->setVisible(true);
        if (jugador2) jugador2->setVisible(true);

        // La visibilidad de los corazones se maneja en actualizarCorazones() en reiniciarJuego
        // Aquí solo se asegura que los items estén en la escena.
        for (QGraphicsPixmapItem *c : corazonesJ1) c->setVisible(true);
        for (QGraphicsPixmapItem *c : corazonesJ2) c->setVisible(true);

        for (Obstaculo *o : obstaculosCasa) o->setVisible(true);
        if (obstaculoNPC) obstaculoNPC->setVisible(true);
    }
}

void Juego::mostrarGanador()
{
    QString mensaje;
    QString titulo;
    QString ganadorColor = "#ecf0f1"; // Color por defecto (blanco)

    bool j1SinVidas = jugador1 && jugador1->getVidas() <= 0;
    bool j2SinVidas = jugador2 && jugador2->getVidas() <= 0;

    // [CORREGIDO] Verificar si todos los targets han sido destruidos.
    bool todosTargetsDestruidos = true;
    for (Obstaculo *target : targets) {
        if (target->obtenerVida() > 0) {
            todosTargetsDestruidos = false;
            break;
        }
    }

    if (j1SinVidas && j2SinVidas)
    {
        titulo = "¡AMBOS DERROTADOS!";
        mensaje = QString("Ambos jugadores han perdido todas sus vidas.\n\n"
                          "Puntaje final:\nJugador 1: %1\nJugador 2: %2")
                      .arg(puntajeJ1).arg(puntajeJ2);
        ganadorColor = "#f39c12"; // Naranja para empate
    }
    else if (j1SinVidas)
    {
        titulo = "¡JUGADOR 2 GANA!";
        mensaje = QString("Jugador 1 perdió todas sus vidas.\n\n"
                          "Puntaje final:\nJugador 1: %1\nJugador 2: %2")
                      .arg(puntajeJ1).arg(puntajeJ2);
        ganadorColor = "#9b59b6"; // Morado para J2
    }
    else if (j2SinVidas)
    {
        titulo = "¡JUGADOR 1 GANA!";
        mensaje = QString("Jugador 2 perdió todas sus vidas.\n\n"
                          "Puntaje final:\nJugador 1: %1\nJugador 2: %2")
                      .arg(puntajeJ1).arg(puntajeJ2);
        ganadorColor = "#2ecc71"; // Verde para J1
    }
    else if (todosTargetsDestruidos) // [CORREGIDO] Usar la nueva variable
    {
        titulo = "¡TARGETS DESTRUIDOS!";
        if (puntajeJ1 > puntajeJ2)
        {
            mensaje = QString("¡Jugador 1 ha obtenido la mayor puntuación!\n\n"
                              "Puntaje final:\nJugador 1: %1\nJugador 2: %2")
                          .arg(puntajeJ1).arg(puntajeJ2);
            ganadorColor = "#2ecc71"; // Verde para J1
        }
        else if (puntajeJ2 > puntajeJ1)
        {
            mensaje = QString("¡Jugador 2 ha obtenido la mayor puntuación!\n\n"
                              "Puntaje final:\nJugador 1: %1\nJugador 2: %2")
                          .arg(puntajeJ1).arg(puntajeJ2);
            ganadorColor = "#9b59b6"; // Morado para J2
        }
        else
        {
            mensaje = QString("¡Empate en puntuación después de destruir los targets!\n\n"
                              "Puntaje final: %1 - %1")
                          .arg(puntajeJ1);
            ganadorColor = "#f39c12"; // Naranja para empate
        }
    }

    // Cambiar la escena al modo Game Over (fondo, ocultar elementos)
    cambiarFondoEscena(true);

    // Mostrar el overlay con la información
    if (finJuegoOverlay) {
        finJuegoOverlay->mostrarMensaje(titulo, mensaje, ganadorColor);
    }

    // Desactivar la entrada de teclado
    vista->setFocusPolicy(Qt::NoFocus);
    if (jugadorActivo) jugadorActivo->clearFocus();
}

void Juego::onReiniciar()
{
    qDebug() << ">>> Reiniciar juego solicitado desde Panel lateral";
    reiniciarJuego();
}

// Slot para manejar el reinicio desde el overlay
void Juego::onReiniciarDesdeOverlay()
{
    qDebug() << ">>> Reiniciar juego solicitado desde Overlay";
    // Ocultar el overlay
    if (finJuegoOverlay) {
        finJuegoOverlay->ocultar();
    }
    // Ejecutar la lógica de reinicio
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

    qDebug() << ">>> Eliminando targets (ex-NPCs)...";
    for (int i = targets.size() - 1; i >= 0; --i)
    {
        Obstaculo *target = targets[i];
        if (target && target->scene() == escena)
        {
            // El target ya se eliminó del escena al eliminar el obstáculo de la casa.
            // Solo necesitamos eliminar la referencia del target
            // La eliminación del objeto 'target' se maneja abajo en obstaculosCasa.
        }
    }
    targets.clear(); // Limpiar la lista de targets


    qDebug() << ">>> Eliminando obstáculos de la casa...";
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
    jugadorActual = 1;
    turnoEnProceso = false;

    // Resetear flags de estado
    hitOccurredThisTurn = false;
    proyectilActivo = false;

    if (labelPuntajeJ1) labelPuntajeJ1->setText("🎮 Jugador 1: 0 pts");
    if (labelPuntajeJ2) labelPuntajeJ2->setText("🎮 Jugador 2: 0 pts");
    if (labelJugador) labelJugador->setText("Turno: Jugador 1");

    // Restaurar el fondo de la escena y visibilidad de elementos del juego
    cambiarFondoEscena(false);

    qDebug() << ">>> Recreando casa...";
    // [CORREGIDO] Llama a crearCasa con los parámetros para recrear las dos casas
    crearCasa(escena->width() * 0.25 - 85, 170, 150); // Casa 1 (Izquierda)
    crearCasa(escena->width() * 0.75 - 85, 170, 150); // Casa 2 (Derecha)

    qDebug() << ">>> Reseteando jugadores...";
    if (jugador1) jugador1->resetear();
    if (jugador2) jugador2->resetear();
    jugadorActivo = jugador1;
    if (jugador1) jugador1->setFocus();
    vista->setFocusPolicy(Qt::StrongFocus); // Restaurar focus en la vista

    actualizarPanel(45, 20);
    actualizarCorazones();

    qDebug() << ">>> reiniciarJuego completado";
}
