#ifndef OBSTACULO_H
#define OBSTACULO_H

#include <QGraphicsRectItem>
#include <QGraphicsTextItem> // Incluir para mostrar texto
#include <QBrush>

class Obstaculo : public QGraphicsRectItem
{
public:
    Obstaculo(int vidaInicial);

    void recibirDano(int d);
    int obtenerVida() const;
    void actualizarColor();

private:
    int vida;
    int vidaMaxima;
    QGraphicsTextItem *labelVida; // Nuevo: Para mostrar la vida
};

#endif // OBSTACULO_H
