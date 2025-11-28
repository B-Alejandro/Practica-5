#ifndef PROYECTIL_H
#define PROYECTIL_H

#include <QGraphicsEllipseItem>
#include <QTimer>
#include <QObject>

class Obstaculo;

class Proyectil : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT

public:
    Proyectil(double x0,
              double y0,
              double velocidad,
              double angulo,
              int jugadorPropietario,
              double gravedad = 98.0);

    ~Proyectil();

signals:
    void impactoDetectado(bool acerto);
    void proyectilFinalizado();
    void jugadorGolpeado(int jugadorNumero);

private slots:
    void actualizar();

private:
    int calcularDano() const;
    void detenerYDestruir();
    void procesarRebote();

    double xInicial;
    double yInicial;
    double v0;
    double angRad;
    double g;
    double tiempo;
    double velocidadX;
    double velocidadY;
    bool impacto;
    bool destruyendo;
    bool eventoEmitido;
    int rebotesRestantes;
    int jugadorOwner;

    QTimer *timer;
};

#endif // PROYECTIL_H
