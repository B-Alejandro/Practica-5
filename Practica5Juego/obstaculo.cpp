#include "obstaculo.h"
#include <QDebug>
#include <QColor>

Obstaculo::Obstaculo(int vidaInicial)
    : vida(vidaInicial), vidaMaxima(vidaInicial)
{
    qDebug() << ">>> Creando Obstaculo con vida:" << vidaInicial;
    setRect(0, 0, 60, 60);
    actualizarColor();
}

void Obstaculo::recibirDano(int d)
{
    qDebug() << ">>> Obstaculo recibe daño:" << d << "Vida anterior:" << vida;

    vida -= d;

    if (vida < 0)
        vida = 0;

    qDebug() << ">>> Vida actual:" << vida;
    actualizarColor();
}

int Obstaculo::obtenerVida() const
{
    return vida;
}

void Obstaculo::actualizarColor()
{
    // Calcular porcentaje de vida restante
    int porcentajeVida = (vida * 100) / vidaMaxima;

    QColor color;

    if (porcentajeVida > 66)
    {
        // Verde - Buena condición
        color = QColor(34, 139, 34);
    }
    else if (porcentajeVida > 33)
    {
        // Amarillo - Daño medio
        color = QColor(255, 165, 0);
    }
    else if (porcentajeVida > 0)
    {
        // Rojo - Crítico
        color = QColor(220, 20, 60);
    }
    else
    {
        // Negro - Destruido
        color = QColor(0, 0, 0);
    }

    setBrush(QBrush(color));
    qDebug() << ">>> Color actualizado. Vida:" << vida << "Porcentaje:" << porcentajeVida << "%";
}
