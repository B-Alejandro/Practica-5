#ifndef FINJUEGOOVERLAY_H
#define FINJUEGOOVERLAY_H

#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QFont>
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QObject> // Necesario para usar Q_OBJECT, señales y slots

class FinDeJuegoOverlay : public QObject, public QGraphicsRectItem
{
    Q_OBJECT // Se requiere para señales y slots

public:
    FinDeJuegoOverlay(qreal ancho, qreal alto, QGraphicsItem *parent = nullptr);

    // Muestra el mensaje en el overlay, incluyendo un color para el título.
    void mostrarMensaje(const QString &titulo, const QString &mensaje, const QString &ganadorColor);
    void ocultar();

signals:
    // Señal emitida cuando el usuario presiona el botón de reiniciar
    void reiniciarJuegoSolicitado();

private slots:
    void onReiniciarClicked(); // Slot interno para el botón

private:
    QGraphicsTextItem *tituloItem;
    QGraphicsTextItem *mensajeItem;
    QFont fuenteComic;
    QPushButton *btnReiniciar;
    QGraphicsProxyWidget *btnProxy; // Permite usar un QPushButton dentro de QGraphicsScene
};

#endif // FINJUEGOOVERLAY_H
