#include "obstaculo.h"

Obstaculo::Obstaculo(int vidaInicial)
{
    vida = vidaInicial;
    setRect(0, 0, 60, 60);
}

void Obstaculo::recibirDano(int d)
{
    vida -= d;

    if (vida < 0)
        vida = 0;
}

int Obstaculo::obtenerVida() const
{
    return vida;
}
