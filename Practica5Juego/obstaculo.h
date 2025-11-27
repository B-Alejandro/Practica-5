#ifndef OBSTACULO_H
#define OBSTACULO_H

#include <QGraphicsRectItem>
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
};

#endif // OBSTACULO_H
