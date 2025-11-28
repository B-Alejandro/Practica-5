#include "obstaculo.h"
#include <QDebug>
#include <QColor>
#include <QFont>
#include <QGraphicsScene> // Para agregar el label a la escena

Obstaculo::Obstaculo(int vidaInicial)
    : vida(vidaInicial), vidaMaxima(vidaInicial)
    , labelVida(new QGraphicsTextItem(this)) // El Obstaculo es el padre del label
{
    qDebug() << ">>> Creando Obstaculo con vida:" << vidaInicial;
    setRect(0, 0, 60, 60);

    // Configurar y posicionar el label de vida
    labelVida->setFont(QFont("Arial", 10, QFont::Bold));
    labelVida->setDefaultTextColor(Qt::white);
    // Posicionar el texto en el centro del obstáculo (ajustes manuales)
    labelVida->setPos(rect().width() / 2 - 10, rect().height() / 2 - 10);
    labelVida->setZValue(1); // Asegurar que esté sobre el rectángulo

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

    // Si la vida es 0, no mostrar el texto, si no, actualizarlo
    if (vida <= 0) {
        labelVida->setVisible(false);
    }
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

    // Actualizar el texto del label de vida
    labelVida->setPlainText(QString::number(vida));
}
