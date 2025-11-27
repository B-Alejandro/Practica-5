#ifndef OBSTACULO_H
#define OBSTACULO_H

#include <QGraphicsRectItem>

class Obstaculo : public QGraphicsRectItem
{
public:
    Obstaculo(int vidaInicial);

    void recibirDano(int d);
    int obtenerVida() const;

private:
    int vida;
};

#endif // OBSTACULO_H
