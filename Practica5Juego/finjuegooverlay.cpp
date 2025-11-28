#include "finjuegooverlay.h"
#include <QBrush>
#include <QDebug>
#include <QColor>
#include <QPen>

// --- Implementación de FinDeJuegoOverlay ---

FinDeJuegoOverlay::FinDeJuegoOverlay(qreal ancho, qreal alto, QGraphicsItem *parent)
    : QGraphicsRectItem(0, 0, ancho, alto, parent)
{
    qDebug() << ">>> FinDeJuegoOverlay: Inicializando overlay";

    // Cargar fuente estilo Comic/Robot
    fuenteComic.setFamily("Comic Sans MS");
    fuenteComic.setBold(true);

    // Fondo oscuro semi-transparente
    setBrush(QBrush(QColor(0, 0, 0, 200))); // Color Negro con 200/255 de opacidad
    setPen(Qt::NoPen); // No queremos borde
    setZValue(500); // Asegura que esté por encima de casi todo (ZValue alto)

    // Título
    tituloItem = new QGraphicsTextItem(this);
    tituloItem->setDefaultTextColor(Qt::white);
    tituloItem->setZValue(501);

    // Mensaje (Puntajes)
    mensajeItem = new QGraphicsTextItem(this);
    mensajeItem->setDefaultTextColor(Qt::white);
    mensajeItem->setZValue(501);

    // Botón Reiniciar (QGraphicsProxyWidget para el QPushButton)
    QPushButton *btnReiniciar = new QPushButton("REINICIAR JUEGO");
    btnReiniciar->setStyleSheet(
        "QPushButton {"
        "   background-color: #3498db;" // Un azul más juguetón
        "   color: white;"
        "   border: none;"
        "   padding: 15px 30px;"
        "   border-radius: 8px;"
        "   font-weight: bold;"
        "   font-size: 16px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #2980b9;" // Azul más oscuro en hover
        "}"
        );
    btnReiniciar->setFixedSize(250, 50);

    // Envuelve el QPushButton en un QGraphicsProxyWidget para añadirlo a la escena
    btnProxy = new QGraphicsProxyWidget(this);
    btnProxy->setWidget(btnReiniciar);
    btnProxy->setZValue(501);

    // Conectar la acción del botón a un slot
    // NOTA: Se conecta el botón (widget) al slot del QObject (FinDeJuegoOverlay)
    connect(btnReiniciar, &QPushButton::clicked, this, &FinDeJuegoOverlay::onReiniciarClicked);

    ocultar(); // Ocultar por defecto
}

void FinDeJuegoOverlay::mostrarMensaje(const QString &titulo, const QString &mensaje, const QString &ganadorColor)
{
    qDebug() << ">>> FinDeJuegoOverlay: Mostrando mensaje de fin de juego";

    // 1. Título (más grande y con color del ganador)
    fuenteComic.setPointSize(48);
    tituloItem->setPlainText(titulo);
    tituloItem->setFont(fuenteComic);
    tituloItem->setDefaultTextColor(QColor(ganadorColor));

    // Ajustar posición al centro
    qreal ancho = rect().width();
    qreal alto = rect().height();

    // Centrar Título
    qreal tituloX = ancho / 2 - tituloItem->boundingRect().width() / 2;
    qreal tituloY = alto / 4;
    tituloItem->setPos(tituloX, tituloY);

    // 2. Mensaje (puntajes)
    fuenteComic.setPointSize(24);
    mensajeItem->setPlainText(mensaje);
    mensajeItem->setFont(fuenteComic);
    mensajeItem->setDefaultTextColor(Qt::white);

    // Centrar Mensaje
    qreal mensajeX = ancho / 2 - mensajeItem->boundingRect().width() / 2;
    qreal mensajeY = tituloY + tituloItem->boundingRect().height() + 20;
    mensajeItem->setPos(mensajeX, mensajeY);

    // 3. Posicionar Botón
    qreal btnX = ancho / 2 - btnProxy->widget()->width() / 2; // Usamos el tamaño del widget dentro del proxy
    qreal btnY = mensajeY + mensajeItem->boundingRect().height() + 50;
    btnProxy->setPos(btnX, btnY);

    setVisible(true);
}

void FinDeJuegoOverlay::ocultar()
{
    setVisible(false);
}

void FinDeJuegoOverlay::onReiniciarClicked()
{
    qDebug() << ">>> Botón Reiniciar presionado en Overlay";
    emit reiniciarJuegoSolicitado(); // Emite la señal a la clase Juego
}
